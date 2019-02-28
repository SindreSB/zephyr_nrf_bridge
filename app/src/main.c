#include <zephyr.h>
#include <misc/printk.h>

//#include <autoconf.h>

#include <device.h>

#include "dexcom/receiver.h"

dexcom_ctx_t dexcom_reveicer_ctx;


void main(void)
{
    printk("Hello World! from %s\n", CONFIG_BOARD);
    test_cc2500(&dexcom_reveicer_ctx);
}
