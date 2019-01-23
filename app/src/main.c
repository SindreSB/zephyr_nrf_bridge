#include <zephyr.h>
#include <misc/printk.h>

//#include <autoconf.h>

#include <device.h>
#include <gpio.h>
#include <spi.h>

// Does this work for NRF52 -yes, but not general enough?
#define GPIO_OUT_DRV_NAME "GPIO_0"
#define GPIO_OUT_PIN  17

#define SPI_DRV_NAME "SPI_0"
#define SPI_CS_PIN 12

#define SEC_TO_TICK(sec) sec * 1200 // Trial and error, not accurate


// SPI buffers
u8_t address_buf[1];
u8_t status_buf[1];
u8_t data_buf[64];

struct spi_buf trans_buf[2];
struct spi_buf recv_buf[2];


void test_gpio(void)
{
    struct device *gpio_out_dev;
	int ret;
	int toggle = 1;

    // Get output driver
    gpio_out_dev = device_get_binding(GPIO_OUT_DRV_NAME);
	if (!gpio_out_dev) {
		printk("Cannot find %s!\n", GPIO_OUT_DRV_NAME);
		return;
	}

    // Configure output
    ret = gpio_pin_configure(gpio_out_dev, GPIO_OUT_PIN, (GPIO_DIR_OUT));
	if (ret) {
		printk("Error configuring pin %d!\n", GPIO_OUT_PIN);
        return;
	}

    // Enter infinite loop toggeling led
    while (1) {
		printk("Toggling pin %d\n", GPIO_OUT_PIN);

		ret = gpio_pin_write(gpio_out_dev, GPIO_OUT_PIN, toggle);
		if (ret) {
			printk("Error set pin %d!\n", GPIO_OUT_PIN);
		}

		if (toggle) {
			toggle = 0;
		} else {
			toggle = 1;
		}

		k_sleep(SEC_TO_TICK(2));
	}
}

void test_spi(void)
{
    struct device *spi;
    struct device *gpio_out_dev;
    int ret;

    // Get SPI driver
    spi = device_get_binding(SPI_DRV_NAME);
    if (!spi) {
        printk("SPI: Device driver not found.\n");
        return;
    }

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

    // CS control config
    struct spi_cs_control cs_ctrl;
    cs_ctrl.gpio_dev = gpio_out_dev;
    cs_ctrl.gpio_pin = SPI_CS_PIN;
    cs_ctrl.delay = 100;

    // SPI config, LSB might be wrong
    struct spi_config config;
    config.frequency = 500000;
    config.slave = SPI_OP_MODE_MASTER;
    config.cs = &cs_ctrl;
    config.operation  = 0 
                        | SPI_OP_MODE_MASTER 
                        | SPI_MODE_CPHA
                        | SPI_TRANSFER_LSB
                        | SPI_WORD_SET(1)
                        | SPI_LINES_SINGLE;

    // Send buffers, single buffer pluss set wrapper
    address_buf[0] = 0xC0 | 0x30; // Address to partnum, which should be 0x80

    struct spi_buf address_spi_buf;
    address_spi_buf.buf = address_buf;
    address_spi_buf.len = 1;
    
    struct spi_buf dummy_spi_buf;
    dummy_spi_buf.buf = NULL;
    dummy_spi_buf.len = 1;

    trans_buf[0] = address_spi_buf;
    trans_buf[1] = dummy_spi_buf;

    struct spi_buf_set trans_buf_set;
    trans_buf_set.buffers = trans_buf;
    trans_buf_set.count = 2;

    // Receive buffer, single buffer pluss set wrapper
    struct spi_buf status_spi_buf;
    status_spi_buf.buf = status_buf;
    status_spi_buf.len = 1;
    
    struct spi_buf received_spi_buf;
    received_spi_buf.buf = data_buf;
    received_spi_buf.len = 1;

    recv_buf[0] = status_spi_buf;
    recv_buf[1] = received_spi_buf;

    struct spi_buf_set recv_buf_set;
    recv_buf_set.buffers = recv_buf;
    recv_buf_set.count = 2;

    // Execute tranceive
    // int spi_transceive(structdevice *dev, conststructspi_config *config, conststructspi_buf_set *tx_bufs, conststructspi_buf_set *rx_bufs)
    ret = spi_transceive(spi, &config, &trans_buf_set, &recv_buf_set);
    if (ret != 0) {
        printk("An error occured during SPI transmission: %d\n", ret);
    }

    // Check statusbit 
    printk("Status byte: %x\n", status_buf[0]);
    printk("Data byte: %x\n", data_buf[0]);

}

void main(void)
{
	printk("Hello World! %s\n", CONFIG_BOARD);
    //test_gpio();
    test_spi();
}
