#include <zephyr.h>
#include <misc/printk.h>

//#include <autoconf.h>

#include <device.h>
#include <gpio.h>

#include "dexcom/receiver.h"

DEXCOM_RECEIVER(dexcom_receiver_ctx);

dexcom_package_t *pkt;

void main(void)
{
    start_cc2500(&dexcom_receiver_ctx);

    while(1){
        pkt = k_fifo_get(dexcom_receiver_ctx.package_queue, K_FOREVER);

        printk("Timestamp: %lld\n", pkt->timestamp);
        printk("Raw: %d\n", pkt->rawIsig);
        printk("Fil: %d\n", pkt->filIsig);

        k_free(pkt);
    }
    
}
