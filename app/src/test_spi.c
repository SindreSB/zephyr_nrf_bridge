#include <zephyr.h>
#include <misc/printk.h>

//#include <autoconf.h>

#include <device.h>
#include <gpio.h>
#include <spi.h>

#include "cc2500/cc2500_spi.h"
#include "cc2500/cc2500_reg.h"
#include "cc2500/cc2500.h"

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


u8_t data_buffer[64];

u8_t rx_buf[10];
u8_t tx_buf[10];

void read_package_from_cc()
{
    printk("GDO0 asserted, which indicates end of packet received\n");

    // Config should have put the device into idle mode at end of package
    // but just to be sure
    cc2500_mode_idle();
    
    u8_t rxbytes, status;
    cc2500_read_register(CC2500_REG_RXBYTES, &rxbytes, &status);

    printk("Status is: %02X\n", status);

    // If overflow, ignore it all and retry
    if (rxbytes & BIT(7)) {
        printk("Overflow in RX FIFO\n");
        
    }
    else {
        u8_t num_bytes_rx = rxbytes & 0x7F; // Mask bit 7
        cc2500_read_burst(CC2500_REG_RXFIFO, data_buffer, num_bytes_rx, NULL);

        printk("%d bytes received\n", num_bytes_rx);
        int i;
        for(i = 0; i < num_bytes_rx; i++) {
            printk("%02X ", data_buffer[i]);
        }
    }    

    printk("Flushing rx fifo\n");
    cc2500_flush_rxfifo();

    printk("Go back into receive\n");
    cc2500_mode_receive(0);
    
    printk("Handler completed\n");
    return;
}

void test_cc2500(void) 
{
    printk("Testing CC2500\n");
    u8_t value, status;

    /*if(!cc2500_verify_osc_stabilization()) {
        printk("CC2500 didn't become ready in time\n");
        return;
    }*/

    cc2500_read_status_reg(CC2500_REG_PARTNUM, &value, &status);
    printk("Status: 0x%02X\n", status);
    printk("Partnum: 0x%02X\n", value);

    k_busy_wait(1000000 *2);

    cc2500_read_status_reg(CC2500_REG_VERSION, &value, &status);
    printk("Status: 0x%02X\n", status);
    printk("Version: 0x%02X\n", value);
    

    // Write required register values
    printk("Writing Dexcom config to device\n");
    cc2500_write_register(CC2500_REG_IOCFG0,	0x01, NULL);
    cc2500_write_register(CC2500_REG_PKTLEN,	0xff, NULL);
    cc2500_write_register(CC2500_REG_PKTCTRL1,	0x04, NULL);
    cc2500_write_register(CC2500_REG_PKTCTRL0,	0x05, NULL);
    cc2500_write_register(CC2500_REG_ADDR,	    0x00, NULL);
    cc2500_write_register(CC2500_REG_CHANNR,	0x00, NULL);
    cc2500_write_register(CC2500_REG_FSCTRL1,	0x0f, NULL);
    cc2500_write_register(CC2500_REG_FSCTRL0,	0x00, NULL);
    cc2500_write_register(CC2500_REG_FREQ2,	    0x5d, NULL);
    cc2500_write_register(CC2500_REG_FREQ1,	    0x44, NULL);
    cc2500_write_register(CC2500_REG_FREQ0,	    0xeb, NULL);
    cc2500_write_register(CC2500_REG_FREND1,	0xb6, NULL);
    cc2500_write_register(CC2500_REG_FREND0,	0x10, NULL);
    cc2500_write_register(CC2500_REG_MDMCFG4,	0x7a, NULL);
    cc2500_write_register(CC2500_REG_MDMCFG3,	0xf8, NULL);
    cc2500_write_register(CC2500_REG_MDMCFG2,	0x73, NULL);
    cc2500_write_register(CC2500_REG_MDMCFG1,	0x23, NULL);
    cc2500_write_register(CC2500_REG_MDMCFG0,	0x3b, NULL);
    cc2500_write_register(CC2500_REG_DEVIATN,	0x40, NULL);
    cc2500_write_register(CC2500_REG_MCSM2,	    0x07, NULL);
    cc2500_write_register(CC2500_REG_MCSM1, 	0x30, NULL);
    cc2500_write_register(CC2500_REG_MCSM0,	    0x18, NULL);
    cc2500_write_register(CC2500_REG_FOCCFG,	0x16, NULL);
    cc2500_write_register(CC2500_REG_FSCAL3,	0xa9, NULL);
    cc2500_write_register(CC2500_REG_FSCAL2,	0x0a, NULL);
    cc2500_write_register(CC2500_REG_FSCAL1,	0x00, NULL);
    cc2500_write_register(CC2500_REG_FSCAL0,	0x11, NULL);
    cc2500_write_register(CC2500_REG_AGCCTRL2,	0x03, NULL);
    cc2500_write_register(CC2500_REG_AGCCTRL1,	0x00, NULL);
    cc2500_write_register(CC2500_REG_AGCCTRL0,	0x91, NULL);
    cc2500_write_register(CC2500_REG_TEST2,	    0x81, NULL);
    cc2500_write_register(CC2500_REG_TEST1,	    0x35, NULL);
    cc2500_write_register(CC2500_REG_TEST0,	    0x0b, NULL);
    cc2500_write_register(CC2500_REG_FOCCFG,	0x0a, NULL);
    cc2500_write_register(CC2500_REG_BSCFG,	    0x6c, NULL);

    printk("Config completed\n");

    printk("Registering callback for GDO0\n");
    cc2500_register_gdo0_handler(read_package_from_cc);
    printk("Handler registered\n");

    printk("Going into RX mode\n");
    cc2500_mode_receive(0);

    printk("All setup completed\n");
}
