#include <zephyr.h>
#include <misc/printk.h>

//#include <autoconf.h>

#include <device.h>

#include "dexcom/receiver.h"

K_FIFO_DEFINE(dexcom_pkt_fifo);
dexcom_ctx_t dexcom_reveicer_ctx = {
    .package_queue = &dexcom_pkt_fifo,
};

dexcom_package_t *pkt;

void main(void)
{
    test_cc2500(&dexcom_reveicer_ctx);

    while(1){
        pkt = k_fifo_get(&dexcom_pkt_fifo, K_FOREVER);

        printk("Timestamp: %lld\n", pkt->timestamp);
        printk("Raw: %d\n", pkt->rawIsig);
        printk("Fil: %d\n", pkt->filIsig);

        k_free(pkt);
    }
    
}
