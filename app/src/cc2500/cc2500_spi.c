
#include <zephyr.h>
#include <misc/printk.h>

#include <device.h>
#include <gpio.h>
#include <spi.h>

#include "cc2500.h"
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


// Forward declare private functions
int cc2500_init(cc2500_ctx_t *ctx);
int cc2500_transceive(cc2500_ctx_t *ctx, u8_t address, u8_t *tx_buf, u8_t tx_count, u8_t *rx_buf, u8_t rx_count, u8_t *status);


int cc2500_send_strobe(cc2500_ctx_t *ctx, u8_t strobe, u8_t *status)
{
    return cc2500_transceive(ctx, CC2500_OFF_READ_SINGLE | strobe, NULL, 0, NULL, 0, status);
}

int cc2500_write_register(cc2500_ctx_t *ctx, u8_t address, u8_t value, u8_t *status)
{
    return cc2500_transceive(ctx, CC2500_OFF_WRITE_SINGLE | address, &value, 1, NULL, 0, status);
}

int cc2500_read_register(cc2500_ctx_t *ctx, u8_t address, u8_t *value, u8_t *status)
{
    return cc2500_transceive(ctx, CC2500_OFF_READ_SINGLE | address, NULL, 0, value, 1, status);
}

int cc2500_read_burst(cc2500_ctx_t *ctx, u8_t address, u8_t *buffer, int count, u8_t *status)
{
    return cc2500_transceive(ctx, CC2500_OFF_READ_BURST | address, buffer, count, NULL, 0, status);
}

int cc2500_read_status_byte(cc2500_ctx_t *ctx, u8_t *status)
{
    return cc2500_transceive(ctx, CC2500_OFF_READ_SINGLE | CC2500_CMD_SNOP, NULL, 0, NULL, 0, status);
}


int cc2500_read_status_reg(cc2500_ctx_t *ctx, u8_t address, u8_t *value, u8_t *status)
{
    return cc2500_transceive(ctx, CC2500_OFF_READ_BURST | address, NULL, 0, value, 1, status);
}


int cc2500_init(cc2500_ctx_t *ctx)
{
    printk("SPI driver name: %s\n", SPI_DRV_NAME);
    printk("SCK: %d, MISO: %d, MOSI: %d, CSn: %d\n", SPI_2_SCK_PIN, SPI_2_MISO_PIN, SPI_2_MOSI_PIN, SPI_CS_PIN);

    int ret = 0;

    // Get output driver
    ctx->gpio_dev = device_get_binding(GPIO_OUT_DRV_NAME);
    if (!(ctx->gpio_dev)) {
            printk("Cannot find %s!\n", GPIO_OUT_DRV_NAME);
            return ret;
    }

    // Configure output
    ret = gpio_pin_configure(ctx->gpio_dev, SPI_CS_PIN, (GPIO_DIR_OUT));
    if (ret != 0) {
        printk("Error configuring pin %d!\n", SPI_CS_PIN);
        return ret;
    }

    // Get SPI driver
    ctx->spi_dev = device_get_binding(SPI_DRV_NAME);
    if (!ctx->spi_dev) {
        printk("SPI: Device driver not found.\n");
        return ret;
    }

    ctx->gdo0_pin = GDO0_PIN;
    ctx->gdo2_pin = GDO2_PIN;

    ctx->spi_conf.frequency = 500000;
    ctx->spi_conf.slave = SPI_OP_MODE_MASTER;
    ctx->spi_conf.cs = NULL;
    ctx->spi_conf.operation  = 0 
                        | SPI_OP_MODE_MASTER 
                        | SPI_TRANSFER_MSB
                        | SPI_WORD_SET(8);

    
    return 0;
}

int cc2500_transceive(cc2500_ctx_t *ctx, u8_t address, u8_t *tx_buf, u8_t tx_count, u8_t *rx_buf, u8_t rx_count, u8_t *status)
{
    int ret = 0;

    // FIRST check that the connection is init.
    if (ctx->gpio_dev == NULL || ctx->spi_dev == NULL) { // Should also ckeck that CC2500 is ON and not in sleep or off
        ret = cc2500_init(ctx);
    }

	struct spi_buf tx_spi_buf[2] = {
		{
			.buf = &address,
			.len = 1,
		},
		{
			.buf = tx_buf,
			.len = tx_count,
		}
	};

	struct spi_buf_set tx_buf_set = {
		.buffers = tx_spi_buf,
        .count = 2,
	};

    struct spi_buf rx_spi_buf[2] = {
        {
            .buf = status,
            .len = 1,
        },
        {
            .buf = rx_buf,
            .len = rx_count,
        }
    };

    struct spi_buf_set rx_buf_set = {
        .buffers = rx_spi_buf,
        .count = 2,
    };

    if (tx_count == 0) {
        tx_buf_set.count = 1;
    }

    if (rx_count == 0) {
        rx_buf_set.count = 1;
    }

    // Pull CSn low to initiate transfer
    ret = gpio_pin_write(ctx->gpio_dev, SPI_CS_PIN, 0);
    if (ret) { printk("Error set pin %d!\n", SPI_CS_PIN); }


    ret = spi_transceive(ctx->spi_dev, &(ctx->spi_conf), &tx_buf_set, &rx_buf_set);
    if (ret != 0) {
        printk("An error occured during SPI transmission: %d\n", ret);
    }
    
    ret = gpio_pin_write(ctx->gpio_dev, SPI_CS_PIN, 1);
    if (ret != 0) {
            printk("Error set pin %d!\n", SPI_CS_PIN);
    }

    //printk("SPI TRANS: addr: 0x%02X, status: 0x%02X\n", address, *status);

    return 0;
}

int cc2500_register_gdo0_handler(cc2500_ctx_t *ctx, gpio_callback_handler_t handler, u8_t edge)
{
    int ret;
    // Configure interrupt on GDO0 and GDO2
    ret = gpio_pin_configure(ctx->gpio_dev, ctx->gdo0_pin, (GPIO_DIR_IN | GPIO_INT | edge));
    if (ret != 0) {
        printk("Error configuring pin %d!\n", GDO0_PIN);
        return ret;
    }

    gpio_init_callback(ctx->gdo0_cb, handler, BIT(ctx->gdo0_pin));

    ret = gpio_add_callback(ctx->gpio_dev, ctx->gdo0_cb);
    if (ret) {
        printk("Error adding callback to GDO0\n");
        return ret;
    }

    ret = gpio_pin_enable_callback(ctx->gpio_dev, ctx->gdo0_pin);
    if (ret) {
        printk("Error enabling callback to GDO0\n");
        return ret;
    }

    return ret;
}

int cc2500_register_gdo2_handler(cc2500_ctx_t *ctx, gpio_callback_handler_t handler, u8_t edge)
{
    int ret;
    // Configure interrupt on GDO0 and GDO2
    ret = gpio_pin_configure(ctx->gpio_dev, ctx->gdo2_pin, (GPIO_DIR_IN | GPIO_INT | edge));
    if (ret != 0) {
        printk("Error configuring pin %d!\n", ctx->gdo2_pin);
        return ret;
    }

    gpio_init_callback(ctx->gdo2_cb, handler, BIT(ctx->gdo2_pin));

    ret = gpio_add_callback(ctx->gpio_dev, ctx->gdo2_cb);

    ret = gpio_pin_enable_callback(ctx->gpio_dev, ctx->gdo2_pin);

    return ret;
}