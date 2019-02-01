#include <zephyr.h>

#include "cc2500_reg.h"
#include "cc2500_spi.h"
#include "cc2500.h"


#define NUM_CHANNELS 4
static u8_t nChannels[NUM_CHANNELS] = { 0, 100, 199, 209 };
static u8_t fOffset[NUM_CHANNELS] = {0xe4, 0xe3, 0xe2, 0xe2};


bool cc2500_verify_osc_stabilization(cc2500_ctx_t *ctx)
{
    u8_t timeout = 100U;
	u8_t status;

	do {
		cc2500_read_status_byte(ctx, &status);
        
		k_busy_wait(1);
		timeout--;
	} while (!(status & BIT(7)) && timeout);

	return !!(status & BIT(7)); 
}

void cc2500_set_mode(cc2500_ctx_t *ctx, u8_t mode, u8_t checksum){
    u8_t state;
    cc2500_send_strobe(ctx, mode, &state);

    while((state & 0x70) != checksum) {
        k_busy_wait(1);
        cc2500_read_status_byte(ctx, &state);
    }
}

int cc2500_mode_idle(cc2500_ctx_t *ctx) {
    // Ensure we are in idle mode first
    cc2500_set_mode(ctx, CC2500_CMD_SIDLE, 0x00);

    return 0;
}

int cc2500_mode_receive(cc2500_ctx_t *ctx, u8_t channel){
    // Ensure we are in idle mode first
    printk("MR: set idle\n");
    cc2500_set_mode(ctx, CC2500_CMD_SIDLE, 0x00);

    printk("MR: flush rx\n");
    // Ensure RX FIFO is clean
    cc2500_send_strobe(ctx, CC2500_CMD_SFRX, NULL);

    printk("MR: set channel and freq\n");
    // Write the channel dependent offset and channel number
    cc2500_write_register(ctx, CC2500_REG_CHANNR, nChannels[channel], NULL);
    cc2500_write_register(ctx, CC2500_REG_FSCTRL0, fOffset[channel], NULL);

    // Set the chip into RX mode
    printk("MR: set rx mode\n");
    cc2500_set_mode(ctx, CC2500_CMD_SRX, 0x10);

    return 0;
}

int cc2500_flush_rxfifo(cc2500_ctx_t *ctx){
    // Ensure we are in idle mode first
    cc2500_set_mode(ctx, CC2500_CMD_SIDLE, 0x00);

    // Ensure RX FIFO is clean
    cc2500_send_strobe(ctx, CC2500_CMD_SFRX, NULL);

    return 0;
}