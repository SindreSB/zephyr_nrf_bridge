#include <zephyr.h>
#include <logging/log.h>

#include "cc2500_reg.h"
#include "cc2500_spi.h"
#include "cc2500.h"

LOG_MODULE_REGISTER(cc2500);

#include <autoconf.h>

#ifdef CONFIG_CC2500

#define NUM_CHANNELS 4
static u8_t nChannels[NUM_CHANNELS] = { 0, 100, 199, 209 };
static int8_t fOffset[NUM_CHANNELS] = {0xFD, 0xFD, 0xFD, 0xFD};


void update_frequency_offset(u8_t channel, int8_t offset){
    fOffset[channel] += offset;
}

bool cc2500_verify_osc_stabilization(cc2500_ctx_t *ctx)
{
    u8_t timeout = 100U;
	u8_t status;

	do {
		cc2500_read_status_byte(ctx, &status);

		k_sleep(100);
		timeout--;
	} while (((status & 0x80) == 0x80) && timeout);

	return (status & 0x80) == 0; 
}

int cc2500_set_mode(cc2500_ctx_t *ctx, u8_t mode, u8_t checksum){
    //LOG_INF("Setting mode: 0x%02X, with checksum: 0x%02X\n", mode, checksum);

    int ret = 0;
    u8_t timeout = 100U;
    u8_t marcstate, state;
    ret = cc2500_send_strobe(ctx, mode, &state);
    if (ret) return ret;

    while(timeout != 0) {
        ret = cc2500_read_status_reg(ctx, CC2500_REG_MARCSTATE, &marcstate, &state);
        if (ret != 0) {
            LOG_INF("An error occured reading register MARCSTATE");
            return ret;
        }
        //LOG_INF("MARCSTATE: 0x%02X\n", marcstate);
        if ((marcstate & (u8_t)0x1F) == checksum) {
            return 0;
        }

        k_sleep(100);
        timeout--;
    }
    
    if (timeout == 0 || ret != 0) {
        LOG_INF("Set mode timed out, ret: %d", ret);
        return ret ? -1 : ret;
    }
    
    return ret;
}

int cc2500_mode_idle(cc2500_ctx_t *ctx) {
    // Ensure we are in idle mode first
    cc2500_set_mode(ctx, CC2500_CMD_SIDLE, 0x01);

    return 0;
}

int cc2500_mode_off(cc2500_ctx_t *ctx) {
    // Send the SOFF strobe, do not try to read backe state
    cc2500_send_strobe(ctx, CC2500_CMD_SXOFF, NULL);

    return 0;
}

int cc2500_mode_receive(cc2500_ctx_t *ctx, u8_t channel){
    // Ensure we are in idle mode first
    cc2500_set_mode(ctx, CC2500_CMD_SIDLE, 0x01);

    // Ensure RX FIFO is clean
    cc2500_send_strobe(ctx, CC2500_CMD_SFRX, NULL);

    // Write the channel dependent offset and channel number
    cc2500_write_register(ctx, CC2500_REG_CHANNR, nChannels[channel], NULL);
    cc2500_write_register(ctx, CC2500_REG_FSCTRL0, fOffset[channel], NULL);

    // Set the chip into RX mode
    cc2500_set_mode(ctx, CC2500_CMD_SRX, 0x0D);

    return 0;
}

int cc2500_flush_rxfifo(cc2500_ctx_t *ctx){
    // Ensure we are in idle mode first
    cc2500_set_mode(ctx, CC2500_CMD_SIDLE, 0x01);

    // Ensure RX FIFO is clean
    cc2500_send_strobe(ctx, CC2500_CMD_SFRX, NULL);

    return 0;
}

#endif // CONFIG_CC2500