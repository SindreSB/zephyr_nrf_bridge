#include <zephyr.h>
#include <logging/log.h>
#include <logging/log_ctrl.h>

//#include <autoconf.h>

#include <device.h>
#include <gpio.h>
#include <spi.h>

#include "../cc2500/cc2500_spi.h"
#include "../cc2500/cc2500_reg.h"
#include "../cc2500/cc2500.h"

#include "../cc2500/utils.h"
#include "utils.h"

// Does this work for NRF52 -yes, but not general enough?
#define GPIO_OUT_DRV_NAME "GPIO_0"
#define GPIO_OUT_PIN  17

#define SPI_DRV_NAME SPI_2_LABEL
#define SPI_CS_PIN 12
// #define SPI_2_LABEL					    DT_NORDIC_NRF_SPI_40023000_LABEL
// #define SPI_2_MISO_PIN					DT_NORDIC_NRF_SPI_40023000_MISO_PIN
// #define SPI_2_MOSI_PIN					DT_NORDIC_NRF_SPI_40023000_MOSI_PIN
// #define SPI_2_SCK_PIN					DT_NORDIC_NRF_SPI_40023000_SCK_PIN

#define SEC_TO_TICK(sec) sec * 1200 // Trial and error, not accurate

LOG_MODULE_REGISTER(test_cc2500);

u8_t data_buffer[64];

// This will have to be replaced by a macro
// Replace cc_ with a given prefix
static gpio_callback_t cc_gdo0_cb;
static gpio_callback_t cc_gdo2_cb;
cc2500_ctx_t cc_ctx = {
    .gdo0_cb = &cc_gdo0_cb,
    .gdo2_cb = &cc_gdo2_cb,
};


void process_dexcom_package(u8_t package_length) {
    printk("Data received: ");
    int i;
    for(i = 0; i < package_length; i++) {
        printk("%02X ", data_buffer[i]);
    }

    // Check first byte, which is package length
    if (data_buffer[0] != 18) {
        LOG_INF("Package length issue");
        return;
    }

    // Check package CRC

    // Check payload CRC
    u8_t crc = compute_crc8_simple(0x2F, &(data_buffer[12]), 7);
    if (crc != 0x00) {
        LOG_INF("CRC failed for payload");
        return;
    }

    uint16_t raw, filtered;
    raw = extractISIG(data_buffer + 12);
    filtered = extractISIG(data_buffer + 14);

    printk("\n");
    printk("Raw: %d\n", raw);
    printk("Fil: %d\n", filtered);
}

void read_package_from_cc(struct k_work *item)
{
    LOG_INF("GDO0 asserted, which indicates end of packet received");

    // Config should have put the device into idle mode at end of package
    // but just to be sure
    cc2500_mode_idle(&cc_ctx);
    
    u8_t rxbytes, status;
    cc2500_read_status_reg(&cc_ctx, CC2500_REG_RXBYTES, &rxbytes, &status);

    LOG_INF("Status is: %02X, rxbytes is: %02X", status, rxbytes);

    // If overflow, ignore it all and retry
    if ((rxbytes & 0x80)) { 
        LOG_INF("Overflow in RX FIFO, ignore data");
        
    }
    else if ((rxbytes & 0x7F) > 0 && (status & 0x07) > 0) {
        LOG_INF("RXBYTES is: %02X", rxbytes);   
        u8_t num_bytes_rx = rxbytes & 0x7F; // Mask bit 7, which is overflow
        LOG_INF("%d bytes received", num_bytes_rx);
        
        cc2500_read_burst(&cc_ctx, CC2500_REG_RXFIFO, data_buffer, num_bytes_rx, NULL);

        process_dexcom_package(num_bytes_rx);

    }    

    LOG_INF("Flushing RX FIFO");
    cc2500_flush_rxfifo(&cc_ctx);

    LOG_INF("Going back into RX mode");
    cc2500_mode_receive(&cc_ctx, 0);
    
    LOG_INF("Handler completed, listening...");
    return;
}

K_WORK_DEFINE(work, read_package_from_cc);
void submit_package_read(struct device *gpiob, struct gpio_callback *cb,
		    u32_t pins)
{
    LOG_INF("Submitting read package job");
    k_work_submit(&work);
}


void test_cc2500(void) 
{
    LOG_INF("Testing CC2500");
    u8_t value, status;

    LOG_INF("Resetting CC2500");
    cc2500_send_strobe(&cc_ctx, CC2500_CMD_SRES, &status);

    LOG_INF("Asserting XTAL is stable");
    if(!cc2500_verify_osc_stabilization(&cc_ctx)) {
        LOG_INF("CC2500 didn't become ready in time");
        return;
    }
    
    LOG_INF("Reading off values to ensure CC2500 is working");
    cc2500_read_status_reg(&cc_ctx, CC2500_REG_PARTNUM, &value, &status);
    LOG_INF("Partnum: 0x%02X, should be 0x80", value);

    cc2500_read_status_reg(&cc_ctx, CC2500_REG_VERSION, &value, &status);
    LOG_INF("Version: 0x%02X, should be 0x03", value);

    // Write required register values
    LOG_INF("Writing Dexcom config to device");
    cc2500_write_register(&cc_ctx, CC2500_REG_PATABLE,      0x00, NULL);
    cc2500_write_register(&cc_ctx, CC2500_REG_IOCFG0,	    0x01, NULL);
    cc2500_write_register(&cc_ctx, CC2500_REG_PKTLEN,	    0xFF, NULL);
    cc2500_write_register(&cc_ctx, CC2500_REG_PKTCTRL1,	    0x04, NULL);
    cc2500_write_register(&cc_ctx, CC2500_REG_PKTCTRL0,	    0x05, NULL);
    cc2500_write_register(&cc_ctx, CC2500_REG_ADDR,	        0x00, NULL);
    cc2500_write_register(&cc_ctx, CC2500_REG_CHANNR,	    0x00, NULL);

    cc2500_write_register(&cc_ctx, CC2500_REG_FSCTRL1,	    0x0F, NULL);
    cc2500_write_register(&cc_ctx, CC2500_REG_FSCTRL0,	    0x00, NULL);
    
    cc2500_write_register(&cc_ctx, CC2500_REG_FREQ2,	    0x5D, NULL);
    cc2500_write_register(&cc_ctx, CC2500_REG_FREQ1,	    0x44, NULL);
    cc2500_write_register(&cc_ctx, CC2500_REG_FREQ0,	    0xEB, NULL);
    
    cc2500_write_register(&cc_ctx, CC2500_REG_FREND1,	    0xB6, NULL);
    cc2500_write_register(&cc_ctx, CC2500_REG_FREND0,	    0x10, NULL);
    
    cc2500_write_register(&cc_ctx, CC2500_REG_MDMCFG4,	    0x7A, NULL);
    cc2500_write_register(&cc_ctx, CC2500_REG_MDMCFG3,	    0xF8, NULL);
    cc2500_write_register(&cc_ctx, CC2500_REG_MDMCFG2,	    0x73, NULL);
    cc2500_write_register(&cc_ctx, CC2500_REG_MDMCFG1,	    0x23, NULL);
    cc2500_write_register(&cc_ctx, CC2500_REG_MDMCFG0,	    0x3B, NULL);
    
    cc2500_write_register(&cc_ctx, CC2500_REG_DEVIATN,	    0x40, NULL);
    
    cc2500_write_register(&cc_ctx, CC2500_REG_MCSM2,	    0x07, NULL);
    cc2500_write_register(&cc_ctx, CC2500_REG_MCSM1, 	    0x30, NULL);
    cc2500_write_register(&cc_ctx, CC2500_REG_MCSM0,	    0x18, NULL);
    
    cc2500_write_register(&cc_ctx, CC2500_REG_FOCCFG,	    0x16, NULL);
    
    cc2500_write_register(&cc_ctx, CC2500_REG_FSCAL3,	    0xA9, NULL);
    cc2500_write_register(&cc_ctx, CC2500_REG_FSCAL2,	    0x0A, NULL);
    cc2500_write_register(&cc_ctx, CC2500_REG_FSCAL1,	    0x00, NULL);
    cc2500_write_register(&cc_ctx, CC2500_REG_FSCAL0,	    0x11, NULL);
    
    cc2500_write_register(&cc_ctx, CC2500_REG_AGCCTRL2,	    0x03, NULL);
    cc2500_write_register(&cc_ctx, CC2500_REG_AGCCTRL1,	    0x00, NULL);
    cc2500_write_register(&cc_ctx, CC2500_REG_AGCCTRL0,	    0x91, NULL);
    
    cc2500_write_register(&cc_ctx, CC2500_REG_TEST2,	    0x81, NULL);
    cc2500_write_register(&cc_ctx, CC2500_REG_TEST1,	    0x35, NULL);
    cc2500_write_register(&cc_ctx, CC2500_REG_TEST0,	    0x0B, NULL);
    
    cc2500_write_register(&cc_ctx, CC2500_REG_FOCCFG,	    0x0A, NULL);
    
    cc2500_write_register(&cc_ctx, CC2500_REG_BSCFG,	    0x6C, NULL);

    LOG_INF("Registering callback for GDO0");
    cc2500_register_gdo0_handler(&cc_ctx, submit_package_read, GPIO_INT_EDGE | GPIO_INT_ACTIVE_HIGH);

    LOG_INF("Going into RX mode");
    cc2500_mode_receive(&cc_ctx, 0);

    LOG_INF("All setup completed, listening...");
}
