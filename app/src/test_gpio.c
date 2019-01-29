#include <zephyr.h>
#include <misc/printk.h>

//#include <autoconf.h>

#include <device.h>
#include <gpio.h>
#include <spi.h>

// Does this work for NRF52 -yes, but not general enough?
#define GPIO_OUT_DRV_NAME "GPIO_0"
#define GPIO_OUT_PIN  17

#define SPI_DRV_NAME "SPI_2"
#define SPI_CS_PIN 12

#define SEC_TO_TICK(sec) sec * 1200 // Trial and error, not accurate




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


void test_button_in(void) {
	struct device *gpio_out_dev;
	int ret;
	int toggle = 1;

    // Get output driver
    gpio_out_dev = device_get_binding(GPIO_OUT_DRV_NAME);
	if (!gpio_out_dev) {
		printk("Cannot find %s!\n", GPIO_OUT_DRV_NAME);
		return;
	}

    // Configure input
    ret = gpio_pin_configure(gpio_out_dev, 13, (GPIO_DIR_IN | GPIO_INT_ACTIVE_LOW | GPIO_PUD_PULL_UP));
	if (ret) {
		printk("Error configuring pin %d!\n", 13);
        return;
	}

	while (1) {
		ret = gpio_pin_read(gpio_out_dev, 13, &toggle);
		if (ret) {
			printk("Failed to read pin with id: %d\n", ret);
			return;
		}

		printk("Pin value is: %d \n", toggle);

		k_sleep(SEC_TO_TICK(2));
	}
}