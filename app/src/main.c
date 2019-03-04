#include <zephyr.h>
#include <misc/printk.h>

//#include <autoconf.h>

#include <device.h>
#include <gpio.h>

#include "dexcom/receiver.h"

DEXCOM_RECEIVER(dexcom_receiver_ctx);
DEXCOM_RECEIVER_TIMER(dexcom_receiver_ctx);
DEXCOM_INTERRUPT_HANDLER(dexcom_receiver_ctx);

dexcom_package_t *pkt;

void init_dexcom() {
    dexcom_receiver_ctx.timeout_timer = &dexcom_receiver_ctx_timer;
    dexcom_receiver_ctx.gdo0_interrupt_handler = &dexcom_receiver_ctx_int_handler;
}

void main(void)
{
    init_dexcom();

    start_cc2500(&dexcom_receiver_ctx);

    while(1){
        pkt = k_fifo_get(dexcom_receiver_ctx.package_queue, K_FOREVER);

        printk("Timestamp: %d\n", pkt->timestamp);
        printk("Raw: %d\n", pkt->rawIsig);
        printk("Fil: %d\n", pkt->filIsig);

        k_free(pkt);
    }
    
}
