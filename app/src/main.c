#include <zephyr.h>
#include <misc/printk.h>

//#include <autoconf.h>

#include <device.h>

#include "test_spi.h"
#include "test_gpio.h"

void main(void)
{
    printk("Hello World! from %s\n", CONFIG_BOARD);
    //test_gpio();
    //test_spi();
    test_cc2500();
    //test_button_in();
}
