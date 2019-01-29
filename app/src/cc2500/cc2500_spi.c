
#include <zephyr.h>
#include <misc/printk.h>

#include <device.h>
#include <gpio.h>
#include <spi.h>

#include "cc2500_reg.h"

// Does this work for NRF52 -yes, but not general enough?
#define GPIO_OUT_DRV_NAME "GPIO_0"
#define GPIO_OUT_PIN  17

#define SPI_DRV_NAME SPI_2_LABEL
#define SPI_CS_PIN 12
// #define SPI_2_LABEL					    DT_NORDIC_NRF_SPI_40023000_LABEL
// #define SPI_2_MISO_PIN					DT_NORDIC_NRF_SPI_40023000_MISO_PIN
// #define SPI_2_MOSI_PIN					DT_NORDIC_NRF_SPI_40023000_MOSI_PIN
// #define SPI_2_SCK_PIN					DT_NORDIC_NRF_SPI_40023000_SCK_PIN


typedef struct device device_t;
typedef struct spi_buf spi_buf_t;
typedef struct spi_buf_set spi_buf_set_t;
typedef struct spi_config spi_config_t;
// Buffers
u8_t tx_data[65];
u8_t rx_data[65];

spi_buf_t tx_spi_buf;
spi_buf_t rx_spi_buf;

spi_buf_set_t tx_buf_set;
spi_buf_set_t rx_buf_set;

device_t *spi_device = NULL;
device_t *gpio_device = NULL;
spi_config_t config;

// Forward declare private functions
int cc2500_init();
int cc2500_transceive(u8_t address, u8_t *tx_buf, u8_t tx_count, u8_t *rx_buf, u8_t rx_count, u8_t *status);
int cc2500_execute_transceive();
void bufcpy(u8_t *from, u8_t *to, u8_t count);

/*

int cc2500_send_strobe(u8_t strobe, u8_t *result, u8_t *status);

int cc2500_write_register(u8_t address, u8_t value, u8_t *status);
int cc2500_read_register(u8_t address, u8_t *value, u8_t *status);

int cc2500_read_status(u8_t address, u8_t *value, u8_t *status);
int cc2500_read_burst(u8_t address, u8_t *buffer, int count);

int cc2500_read_status_byte(u8_t * status);

*/

int cc2500_read_status_reg(u8_t address, u8_t *value, u8_t *status)
{
    cc2500_transceive( CC2500_OFF_READ_BURST | address, NULL, 0, value, 1, status);
    return 0;
}


int cc2500_init()
{
    printk("Testing SPI\n");
    printk("SPI driver name: %s\n", SPI_DRV_NAME);
    printk("SCK: %d, MISO: %d, MOSI: %d\n", SPI_2_SCK_PIN, SPI_2_MISO_PIN, SPI_2_MOSI_PIN);

    int ret = 0;

    // Get output driver
    gpio_device = device_get_binding(GPIO_OUT_DRV_NAME);
    if (!gpio_device) {
            printk("Cannot find %s!\n", GPIO_OUT_DRV_NAME);
            return ret;
    }

    // Configure output
    ret = gpio_pin_configure(gpio_device, SPI_CS_PIN, (GPIO_DIR_OUT));
    if (ret) {
        printk("Error configuring pin %d!\n", GPIO_OUT_PIN);
        return ret;
    }

    // Get SPI driver
    spi_device = device_get_binding(SPI_DRV_NAME);
    if (!spi_device) {
        printk("SPI: Device driver not found.\n");
        return ret;
    }

    config.frequency = 500000;
    config.slave = SPI_OP_MODE_MASTER;
    config.cs = NULL;
    config.operation  = 0 
                        | SPI_OP_MODE_MASTER 
                        | SPI_TRANSFER_MSB
                        | SPI_WORD_SET(8);

    tx_spi_buf.buf = tx_data;
    tx_spi_buf.len = 0;

    tx_buf_set.buffers = &tx_spi_buf;
    tx_buf_set.count = 1;

    rx_spi_buf.buf = rx_data;
    rx_spi_buf.len = 0;

    rx_buf_set.buffers = &rx_spi_buf;
    rx_buf_set.count = 1;

    return 0;
}

void bufcpy(u8_t *from, u8_t *to, u8_t count){
    if (from == NULL || to == NULL) {
        return;
    }

    u8_t i;
    for(i = 0; i < count; i++) {
        to[i] = from[i];
    }
}

int cc2500_execute_transceive() {
    int ret = 0;

    u32_t chipNotReady = 1;

    // Read MISO pin first to see if it is high initially
    ret = gpio_pin_read(gpio_device, SPI_2_MISO_PIN, &chipNotReady);
    if (ret != 0) { printk("An error occured reading MISO"); }
    printk("Value of MISO before pulling down CSn: %d\n", chipNotReady);

    // Execute tranceive
    ret = gpio_pin_write(gpio_device, GPIO_OUT_PIN, 0);
    if (ret) { printk("Error set pin %d!\n", GPIO_OUT_PIN); }

    // Wait for MISO to go low
    while(chipNotReady) {
      ret = gpio_pin_read(gpio_device, 28, &chipNotReady);

      if (ret != 0) {
        printk("An error occured pulling MISO");
        break;
      }
    }

    // int spi_transceive(structdevice *dev, conststructspi_config *config, conststructspi_buf_set *tx_bufs, conststructspi_buf_set *rx_spi_bufbufs)
    ret = spi_transceive(spi_device, &config, &tx_buf_set, &rx_buf_set);
    if (ret != 0) {
        printk("An error occured during SPI transmission: %d\n", ret);
    }
    
    ret = gpio_pin_write(gpio_device, GPIO_OUT_PIN, 1);
    if (ret) {
            printk("Error set pin %d!\n", GPIO_OUT_PIN);
    }

    return ret;
}

int cc2500_transceive(u8_t address, u8_t *tx_buf, u8_t tx_count, u8_t *rx_buf, u8_t rx_count, u8_t *status)
{
    int ret = 0;

    // FIRST check that the connection is init.
    if (gpio_device == NULL || spi_device == NULL) {
        ret = cc2500_init();
    }

    tx_data[0] = address;
    bufcpy(tx_buf, tx_data + 1, tx_count);
    tx_spi_buf.len = tx_count + 1;

    // Should clear memory in rx buf? 
    rx_spi_buf.len = rx_count + 1;


    ret = cc2500_execute_transceive();

    bufcpy(rx_data + 1, rx_buf, rx_count);

    if (status != NULL) {
        *status = rx_data[0];
    }

    return 0;
}

