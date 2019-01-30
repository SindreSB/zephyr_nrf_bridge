#include <zephyr.h>
#include <misc/printk.h>

//#include <autoconf.h>

#include <device.h>
#include <gpio.h>
#include <spi.h>

#include "cc2500/cc2500_spi.h"
#include "cc2500/cc2500_reg.h"

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


// SPI buffers
u8_t tx_buf[10];
u8_t rx_buf[10];

struct spi_buf trans_buf[10];
struct spi_buf recv_buf[10];

void test_cc2500(void) 
{
    printk("Testing CC2500\n");
    u8_t value, status;

    cc2500_read_status_reg(CC2500_REG_PARTNUM, &value, &status);
    printk("Status: 0x%02X\n", status);
    printk("Partnum: 0x%02X\n", value);

    cc2500_read_status_reg(CC2500_REG_VERSION, &value, &status);
    printk("Status: 0x%02X\n", status);
    printk("Version: 0x%02X\n", value);
    
}

void test_spi(void)
{
    printk("Testing SPI\n");
    printk("SPI driver name: %s\n", SPI_DRV_NAME);
    printk("SCK: %d, MISO: %d, MOSI: %d\n", SPI_2_SCK_PIN, SPI_2_MISO_PIN, SPI_2_MOSI_PIN);

    struct device *spi;
    struct device *gpio_out_dev;
    int ret;


    // Get output driver
    gpio_out_dev = device_get_binding(GPIO_OUT_DRV_NAME);
    if (!gpio_out_dev) {
            printk("Cannot find %s!\n", GPIO_OUT_DRV_NAME);
            return;
    }

    // Configure output
    ret = gpio_pin_configure(gpio_out_dev, SPI_CS_PIN, (GPIO_DIR_OUT));
    if (ret) {
        printk("Error configuring pin %d!\n", GPIO_OUT_PIN);
        return;
    }

    ret = gpio_pin_write(gpio_out_dev, GPIO_OUT_PIN, 1);
    if (ret) {
            printk("Error set pin %d!\n", GPIO_OUT_PIN);
    }

    

    // Get SPI driver
    spi = device_get_binding(SPI_DRV_NAME);
    if (!spi) {
        printk("SPI: Device driver not found.\n");
        return;
    }
    // CS control config
    /*struct spi_cs_control cs_ctrl;cs_ctrl.gpio_dev = gpio_out_dev;cs_ctrl.gpio_pin = SPI_CS_PIN;cs_ctrl.delay = 100;*/

    // SPI config, LSB might be wrong
    struct spi_config config;
    config.frequency = 500000;
    config.slave = SPI_OP_MODE_MASTER;
    config.cs = NULL;
    config.operation  = 0 
                        | SPI_OP_MODE_MASTER 
                        | SPI_TRANSFER_MSB
                        | SPI_WORD_SET(8);

    // Send buffers, single buffer pluss set wrapper
    tx_buf[0] = 0xC0 | 0x30; // Address to partnum, which should be 0x80
    tx_buf[1] = 0x0; // Dummydata while receiving

    struct spi_buf tx_spi_buf;
    tx_spi_buf.buf = tx_buf;
    tx_spi_buf.len = 2;

    trans_buf[0] = tx_spi_buf;

    struct spi_buf_set trans_buf_set;
    trans_buf_set.buffers = trans_buf;
    trans_buf_set.count = 1;

    // Receive buffer, single buffer pluss set wrapper
    struct spi_buf rx_spi_buf;
    rx_spi_buf.buf = rx_buf;
    rx_spi_buf.len = 2;

    recv_buf[0] = rx_spi_buf;

    struct spi_buf_set recv_buf_set;
    recv_buf_set.buffers = recv_buf;
    recv_buf_set.count = 1;
    


    u32_t chipNotReady = 1;
    // Read MISO pin first to see if it is high initially
    ret = gpio_pin_read(gpio_out_dev, SPI_2_MISO_PIN, &chipNotReady);
    if (ret != 0) {
      printk("An error occured reading MISO");
    }
    printk("Value of MISO before pulling down CSn: %d\n", chipNotReady);

    // Execute tranceive
    ret = gpio_pin_write(gpio_out_dev, GPIO_OUT_PIN, 0);
    if (ret) {
            printk("Error set pin %d!\n", GPIO_OUT_PIN);
    }

    // Wait for MISO to go low
    while(chipNotReady) {
      ret = gpio_pin_read(gpio_out_dev, 28, &chipNotReady);

      if (ret != 0) {
        printk("An error occured pulling MISO");
        break;
      }
    }
    //gpio_pin_read(struct device *port, u32_t pin, u32_t *value)


    // int spi_transceive(structdevice *dev, conststructspi_config *config, conststructspi_buf_set *tx_bufs, conststructspi_buf_set *rx_spi_bufbufs)
    ret = spi_transceive(spi, &config, &trans_buf_set, &recv_buf_set);
    if (ret != 0) {
        printk("An error occured during SPI transmission: %d\n", ret);
    }
    
    ret = gpio_pin_write(gpio_out_dev, GPIO_OUT_PIN, 1);
    if (ret) {
            printk("Error set pin %d!\n", GPIO_OUT_PIN);
    }

    // Check statusbit 
    printk("Status byte: 0x%02X\n", rx_buf[0]);
    printk("Data byte: 0x%02X\n", rx_buf[1]);

    return;
}