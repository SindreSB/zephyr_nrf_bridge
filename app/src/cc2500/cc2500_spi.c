
#ifdef CONFIG_CC2500

#include <zephyr.h>
#include <logging/log.h>
#include <misc/printk.h>

#include <device.h>
#include <gpio.h>
#include <spi.h>

#include "cc2500.h"
#include "cc2500_spi.h"
#include "cc2500_reg.h"

// Includes defines for used pins
#include "config.h"

LOG_MODULE_REGISTER(cc2500_spi);


int cc2500_init(cc2500_ctx_t *ctx)
{
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


int cc2500_register_handler(cc2500_ctx_t *ctx, int pin, gpio_callback_t *cb, gpio_callback_handler_t handler, u8_t edge)
{
    int ret;
    ret = gpio_pin_configure(ctx->gpio_dev, pin, (GPIO_DIR_IN | GPIO_INT | edge));
    if (ret != 0) {
        printk("Error configuring pin %d!\n", GDO0_PIN);
        return ret;
    }

    gpio_init_callback(cb, handler, BIT(pin));

    ret = gpio_add_callback(ctx->gpio_dev, cb);
    if (ret) {
        printk("Error adding callback to GDO0\n");
        return ret;
    }

    ret = gpio_pin_enable_callback(ctx->gpio_dev, pin);
    if (ret) {
        printk("Error enabling callback to GDO0\n");
        return ret;
    }

    return ret;
}

int cc2500_register_gdo0_handler(cc2500_ctx_t *ctx, gpio_callback_handler_t handler, u8_t edge)
{
    return cc2500_register_handler(ctx, ctx->gdo0_pin, ctx->gdo0_cb, handler, edge);   
}

int cc2500_register_gdo2_handler(cc2500_ctx_t *ctx, gpio_callback_handler_t handler, u8_t edge)
{
    return cc2500_register_handler(ctx, ctx->gdo2_pin, ctx->gdo2_cb, handler, edge);
}

#endif // CONFIG_CC2500