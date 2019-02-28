#include <zephyr.h>
#include <misc/printk.h>

//#include <autoconf.h>

#include <device.h>
#include <gpio.h>

#include "dexcom/receiver.h"

K_FIFO_DEFINE(dexcom_pkt_fifo);
u8_t data_buffer[64];

// This will have to be replaced by a macro
// Replace cc_ with a given prefix
static gpio_callback_t cc_gdo0_cb;
static gpio_callback_t cc_gdo2_cb;
cc2500_ctx_t cc_context = {
    .gdo0_cb = &cc_gdo0_cb,
    .gdo2_cb = &cc_gdo2_cb,
};
dexcom_ctx_t dexcom_receiver_ctx = {
    .package_queue = &dexcom_pkt_fifo,
    .data_buffer = data_buffer,
    .cc_ctx = &cc_context,
};

dexcom_package_t *pkt;

void main(void)
{
    start_cc2500(&dexcom_receiver_ctx);

    while(1){
        pkt = k_fifo_get(&dexcom_pkt_fifo, K_FOREVER);

        printk("Timestamp: %lld\n", pkt->timestamp);
        printk("Raw: %d\n", pkt->rawIsig);
        printk("Fil: %d\n", pkt->filIsig);

        k_free(pkt);
    }
    
}
