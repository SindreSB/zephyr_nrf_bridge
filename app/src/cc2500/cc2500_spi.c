
#include <zephyr.h>
#include <misc/printk.h>

#include <device.h>
#include <gpio.h>
#include <spi.h>

#include "cc2500_spi.h"
#include "cc2500_reg.h"

// Does this work for NRF52 -yes, but not general enough?
#define GPIO_OUT_DRV_NAME "GPIO_0"
#define SPI_DRV_NAME SPI_2_LABEL
#define SPI_CS_PIN 17
// #define SPI_2_LABEL					    DT_NORDIC_NRF_SPI_40023000_LABEL
// #define SPI_2_MISO_PIN					DT_NORDIC_NRF_SPI_40023000_MISO_PIN
// #define SPI_2_MOSI_PIN					DT_NORDIC_NRF_SPI_40023000_MOSI_PIN
// #define SPI_2_SCK_PIN					DT_NORDIC_NRF_SPI_40023000_SCK_PIN

#define GDO0_PIN 30
#define GDO2_PIN 31

typedef struct device device_t;
typedef struct gpio_callback gpio_callback_t;

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
spi_config_t config;

device_t *gpio_device = NULL;
static gpio_callback_t gpio_cb;

InterruptHandler gdo0_handler = NULL;
InterruptHandler gdo2_handler = NULL;

// Forward declare private functions
int cc2500_init();
int cc2500_transceive(u8_t address, u8_t *tx_buf, u8_t tx_count, u8_t *rx_buf, u8_t rx_count, u8_t *status);
int cc2500_execute_transceive();

void bufcpy(u8_t *from, u8_t *to, u8_t count);


int cc2500_send_strobe(u8_t strobe, u8_t *status)
{
    return cc2500_transceive( CC2500_OFF_READ_SINGLE | strobe, NULL, 0, NULL, 0, status);
}

int cc2500_write_register(u8_t address, u8_t value, u8_t *status)
{
    return cc2500_transceive( CC2500_OFF_WRITE_SINGLE | address, &value, 1, NULL, 0, status);
}

int cc2500_read_register(u8_t address, u8_t *value, u8_t *status)
{
    return cc2500_transceive( CC2500_OFF_READ_SINGLE | address, NULL, 0, value, 1, status);
}

int cc2500_read_burst(u8_t address, u8_t *buffer, int count, u8_t *status)
{
    return cc2500_transceive( CC2500_OFF_READ_BURST | address, buffer, count, NULL, 0, status);
}

int cc2500_read_status_byte(u8_t *status)
{
    return cc2500_transceive( CC2500_OFF_READ_SINGLE | CC2500_CMD_SNOP, NULL, 0, NULL, 0, status);
}


int cc2500_read_status_reg(u8_t address, u8_t *value, u8_t *status)
{
    return cc2500_transceive( CC2500_OFF_READ_BURST | address, NULL, 0, value, 1, status);
}

void gdo_interrupt(device_t *gpio_dev, gpio_callback_t *cb, u32_t pins)
{
    if (pins & (BIT(GDO0_PIN))) {
        printk("GDO0 generated an interrupt");
        if(gdo0_handler != NULL ){
            (*gdo0_handler)();
        }
    }
    if (pins & (BIT(GDO2_PIN))) {
        printk("GDO2 generated an interrupt");
        if(gdo2_handler != NULL ){
            (*gdo2_handler)();
        }
    }
}

int cc2500_init()
{
    printk("SPI driver name: %s\n", SPI_DRV_NAME);
    printk("SCK: %d, MISO: %d, MOSI: %d, CSn: %d\n", SPI_2_SCK_PIN, SPI_2_MISO_PIN, SPI_2_MOSI_PIN, SPI_CS_PIN);

    int ret = 0;

    // Get output driver
    gpio_device = device_get_binding(GPIO_OUT_DRV_NAME);
    if (!gpio_device) {
            printk("Cannot find %s!\n", GPIO_OUT_DRV_NAME);
            return ret;
    }

    // Configure output
    ret = gpio_pin_configure(gpio_device, SPI_CS_PIN, (GPIO_DIR_OUT));
    if (ret != 0) {
        printk("Error configuring pin %d!\n", SPI_CS_PIN);
        return ret;
    }

    // Configure interrupt on GDO0 and GDO2
    ret = gpio_pin_configure(gpio_device, GDO0_PIN, (GPIO_DIR_IN | GPIO_INT | GPIO_INT_ACTIVE_HIGH));
    if (ret != 0) {
        printk("Error configuring pin %d!\n", GDO0_PIN);
        return ret;
    }

    ret = gpio_pin_configure(gpio_device, GDO2_PIN, (GPIO_DIR_IN | GPIO_INT | GPIO_INT_ACTIVE_HIGH));
    if (ret != 0) {
        printk("Error configuring pin %d!\n", GDO2_PIN);
        return ret;
    }

    gpio_init_callback(&gpio_cb, gdo_interrupt, BIT(GDO0_PIN) | BIT(GDO2_PIN));
    gpio_add_callback(gpio_device, &gpio_cb);


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

    // Pull CSn low to initiate transfer
    ret = gpio_pin_write(gpio_device, SPI_CS_PIN, 0);
    if (ret) { printk("Error set pin %d!\n", SPI_CS_PIN); }


    ret = spi_transceive(spi_device, &config, &tx_buf_set, &rx_buf_set);
    if (ret != 0) {
        printk("An error occured during SPI transmission: %d\n", ret);
    }
    

    ret = gpio_pin_write(gpio_device, SPI_CS_PIN, 1);
    if (ret != 0) {
            printk("Error set pin %d!\n", SPI_CS_PIN);
    }

    return ret;
}

int cc2500_transceive(u8_t address, u8_t *tx_buf, u8_t tx_count, u8_t *rx_buf, u8_t rx_count, u8_t *status)
{
    int ret = 0;

    // FIRST check that the connection is init.
    if (gpio_device == NULL || spi_device == NULL) { // Should also ckeck that CC2500 is ON and not in sleep or off
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

int cc2500_register_gdo0_handler(InterruptHandler handler)
{
    gdo0_handler = handler;
    return gpio_pin_enable_callback(gpio_device, GDO0_PIN);
}

int cc2500_register_gdo2_handler(InterruptHandler handler)
{
    gdo2_handler = handler;
    return gpio_pin_enable_callback(gpio_device, GDO2_PIN);
}