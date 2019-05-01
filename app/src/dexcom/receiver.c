#include <zephyr.h>
#include <logging/log.h>
#include <logging/log_ctrl.h>

//#include <autoconf.h>

#include <device.h>
#include <gpio.h>
#include <spi.h>

#include "../cc2500/cc2500_spi.h"
#include "../cc2500/cc2500_reg.h"
#include "../cc2500/cc2500.h"

#include "receiver.h"
#include "utils.h"

LOG_MODULE_REGISTER(dex_receiver);


/**
 * @brief Start and configure the CC2500 for package receiption
 * 
 * 
 * @param ctx The Dexcom receiver context to start
 */
void start_cc2500(dexcom_ctx_t *ctx) {
    // First configure the cc2500
    int ret;
    ret = configure_cc2500(ctx);
    if (ret) {
        LOG_ERR("Unable to start CC2500");
        return;
    }

    // Set up hanler for package asserted
    cc2500_register_gdo0_handler(ctx->cc_ctx, ctx->gdo0_interrupt_handler, GPIO_INT_EDGE | GPIO_INT_ACTIVE_HIGH);
    
    // Go into receive on channel 0
    ctx->current_channel = 0;
    cc2500_mode_receive(ctx->cc_ctx, ctx->current_channel);
}

/**
 * @brief Configure the CC2500 for receiving values from a G4
 * 
 * @param ctx The dexcom receiver context
 */
int configure_cc2500(dexcom_ctx_t *ctx) 
{
    LOG_DBG("Asserting XTAL is stable");
    if(!cc2500_verify_osc_stabilization(ctx->cc_ctx)) {
        LOG_ERR("CC2500 didn't become ready in time");
        return -1;
    }
    
    u8_t value, status;

    LOG_DBG("Reading off values to ensure CC2500 is working");
    cc2500_read_status_reg(ctx->cc_ctx, CC2500_REG_PARTNUM, &value, &status);
    LOG_DBG("Partnum: 0x%02X, should be 0x80", value);

    cc2500_read_status_reg(ctx->cc_ctx, CC2500_REG_VERSION, &value, &status);
    LOG_DBG("Version: 0x%02X, should be 0x03", value);

    // Write required register values
    LOG_DBG("Writing Dexcom config to device");
    cc2500_write_register(ctx->cc_ctx, CC2500_REG_PATABLE,     0x00, NULL);
    cc2500_write_register(ctx->cc_ctx, CC2500_REG_IOCFG0,	    0x01, NULL);
    cc2500_write_register(ctx->cc_ctx, CC2500_REG_PKTLEN,	    0xFF, NULL);
    cc2500_write_register(ctx->cc_ctx, CC2500_REG_PKTCTRL1,	0x04, NULL);
    cc2500_write_register(ctx->cc_ctx, CC2500_REG_PKTCTRL0,	0x05, NULL);
    cc2500_write_register(ctx->cc_ctx, CC2500_REG_ADDR,	    0x00, NULL);
    cc2500_write_register(ctx->cc_ctx, CC2500_REG_CHANNR,	    0x00, NULL);

    cc2500_write_register(ctx->cc_ctx, CC2500_REG_FSCTRL1,	    0x0F, NULL);
    cc2500_write_register(ctx->cc_ctx, CC2500_REG_FSCTRL0,	    0x00, NULL);
    
    cc2500_write_register(ctx->cc_ctx, CC2500_REG_FREQ2,	    0x5D, NULL);
    cc2500_write_register(ctx->cc_ctx, CC2500_REG_FREQ1,	    0x44, NULL);
    cc2500_write_register(ctx->cc_ctx, CC2500_REG_FREQ0,	    0xEB, NULL);
    
    cc2500_write_register(ctx->cc_ctx, CC2500_REG_FREND1,	    0xB6, NULL);
    cc2500_write_register(ctx->cc_ctx, CC2500_REG_FREND0,	    0x10, NULL);
    
    cc2500_write_register(ctx->cc_ctx, CC2500_REG_MDMCFG4,	    0x7A, NULL);
    cc2500_write_register(ctx->cc_ctx, CC2500_REG_MDMCFG3,	    0xF8, NULL);
    cc2500_write_register(ctx->cc_ctx, CC2500_REG_MDMCFG2,	    0x73, NULL);
    cc2500_write_register(ctx->cc_ctx, CC2500_REG_MDMCFG1,	    0x23, NULL);
    cc2500_write_register(ctx->cc_ctx, CC2500_REG_MDMCFG0,	    0x3B, NULL);
    
    cc2500_write_register(ctx->cc_ctx, CC2500_REG_DEVIATN,	    0x40, NULL);
    
    cc2500_write_register(ctx->cc_ctx, CC2500_REG_MCSM2,	    0x07, NULL);
    cc2500_write_register(ctx->cc_ctx, CC2500_REG_MCSM1, 	    0x30, NULL);
    cc2500_write_register(ctx->cc_ctx, CC2500_REG_MCSM0,	    0x18, NULL);
    
    cc2500_write_register(ctx->cc_ctx, CC2500_REG_FOCCFG,	    0x16, NULL);
    
    cc2500_write_register(ctx->cc_ctx, CC2500_REG_FSCAL3,	    0xA9, NULL);
    cc2500_write_register(ctx->cc_ctx, CC2500_REG_FSCAL2,	    0x0A, NULL);
    cc2500_write_register(ctx->cc_ctx, CC2500_REG_FSCAL1,	    0x00, NULL);
    cc2500_write_register(ctx->cc_ctx, CC2500_REG_FSCAL0,	    0x11, NULL);
    
    cc2500_write_register(ctx->cc_ctx, CC2500_REG_AGCCTRL2,    0x03, NULL);
    cc2500_write_register(ctx->cc_ctx, CC2500_REG_AGCCTRL1,    0x00, NULL);
    cc2500_write_register(ctx->cc_ctx, CC2500_REG_AGCCTRL0,    0x91, NULL);
    
    cc2500_write_register(ctx->cc_ctx, CC2500_REG_TEST2,	    0x81, NULL);
    cc2500_write_register(ctx->cc_ctx, CC2500_REG_TEST1,	    0x35, NULL);
    cc2500_write_register(ctx->cc_ctx, CC2500_REG_TEST0,	    0x0B, NULL);
    
    cc2500_write_register(ctx->cc_ctx, CC2500_REG_FOCCFG,	    0x0A, NULL);
    
    cc2500_write_register(ctx->cc_ctx, CC2500_REG_BSCFG,	    0x6C, NULL);

    LOG_INF("Configuration completed");
    return 0;
}

void handle_no_package_received(dexcom_ctx_t *ctx) {
    // In case this was caused by bad package
    k_timer_stop(ctx->timeout_timer);

    if (ctx->windows_missed == 1 || ctx->last_package_received == -1) {
        ctx->current_channel = 0;
        cc2500_mode_receive(ctx->cc_ctx, ctx->current_channel);
    }
    else {
        ctx->windows_missed++;
        enter_sleep(ctx);
    }   
}

/**
 * @brief Handle event in the Dexcom receiver statemachine
 * 
 * @param ctx The dexcom receiver context
 * @param event The event that occured
 */
void handle_dexcom_event(dexcom_ctx_t *ctx, dexcom_event_t event)
{
    if (event.type == DE_PACKAGE_RECEIVED) {
        LOG_INF("VALID_EVENT:  Package on channel %d", ctx->current_channel);
        // Package received -> put in package fifo
        ctx->windows_missed = 0;
        // Set last received
        u32_t last_received = ((dexcom_package_t *) event.data)->timestamp;
        u32_t channel_compensation = (ctx->current_channel * DE_TIME_BETWEEN_CHANNEL);
        ctx->last_package_received = last_received - channel_compensation;
        LOG_INF("Received package at: %d (%d)", ctx->last_package_received, ctx->last_package_received + DE_TIME_BETWEEN_CHANNEL * ctx->current_channel);

        // Set sleeptimer
        enter_sleep(ctx);

        // Deliver package to handler
        ctx->package_callback((dexcom_package_t*)event.data);

        // Free the memory of the package
        k_free(event.data);

        // Handler complete
        return;
    }
    else if (event.type == DE_PACKAGE_INVALID) 
    {
        LOG_INF("INVALID_EVENT: Invalid package at %d", ctx->last_interrupt_time);
        if (ctx->current_channel == 3) {
            handle_no_package_received(ctx);
        } 
        else {
            // Go to next channels
            ctx->current_channel++;
            cc2500_mode_receive(ctx->cc_ctx, ctx->current_channel);

            // Use a long delay, in case bad window alignment, but do not automatically jump
            k_timer_start(ctx->timeout_timer, K_MSEC(DE_TIME_ASSUME_MISSED), 0);
        }

        return;
    }
    else if (event.type == DE_SLEEP_TIMEOUT)
    {
        LOG_INF("SLEEP_EVENT: Sleep timeout");
        // We need to configure CC again
        configure_cc2500(ctx);

        // Go into receive
        ctx->current_channel = 0;
        cc2500_mode_receive(ctx->cc_ctx, 0);

        ctx->long_sleep = false;

        u32_t missed_window_compesation = ctx->windows_missed * DE_TIME_BETWEEN_TRANS; 
        u32_t next_expected = ctx->last_package_received + DE_TIME_BETWEEN_TRANS + missed_window_compesation;
        LOG_INF("Expecting next transmission at: %d", next_expected);
        
        // After sleep, because of clock drift, use a long timeout, and only change channel if bad package is received
        u32_t first_timeout = (next_expected - k_uptime_get_32()) + (DE_TIME_ASSUME_MISSED);
        LOG_INF("Will timout in: %d", first_timeout);
        k_timer_start(ctx->timeout_timer, K_MSEC(first_timeout), 0);

        LOG_INF("System is awake after sleep at %d", k_uptime_get_32());
        return;
    }
    else if (event.type == DE_CHANNEL_TIMEOUT) 
    {
        // Channel timeout means we assume we lost all transmissions
        handle_no_package_received(ctx);
        
        return;
    }
}


void enter_sleep(dexcom_ctx_t *ctx)
{
    // Send SXOFF
    cc2500_mode_off(ctx->cc_ctx);

    // Calculate sleeptime
    u32_t sleeptime = DE_TIME_BETWEEN_TRANS - DE_TIME_BETWEEN_CHANNEL * (ctx->current_channel + 1) - DE_TIME_WAKEUP_MARGIN;

    // Set timer
    k_timer_start(ctx->timeout_timer, K_MSEC(sleeptime), 0);

    ctx->long_sleep = true;

    LOG_INF("Sleep timer set");
    return;
}


void process_rx_data(dexcom_ctx_t *ctx, u8_t length) 
{
    
    // Check first byte, which is package length and that there are enough bytes left
    if (ctx->data_buffer[0] != 18 || length < 18) {
        LOG_INF("Package length issue");
        HANDLE_PACKAGE_INVALID(ctx);
        return;
    }

    // TODO: Check package CRC

    // Check payload CRC
    u8_t crc = compute_crc8_simple(0x2F, ctx->data_buffer + 12, 7);
    if (crc != 0x00) {
        LOG_INF("CRC failed for payload");
        HANDLE_PACKAGE_INVALID(ctx);
        return;
    }

    dexcom_package_t *package_ptr = k_malloc(sizeof(dexcom_package_t));
    package_ptr->timestamp = ctx->last_interrupt_time;
    package_ptr->transmitterId = 0;
    package_ptr->rawIsig = extractISIG(ctx->data_buffer + 12);
    package_ptr->filIsig = extractISIG(ctx->data_buffer + 14);
    package_ptr->batLevel = 0;

    dexcom_event_t event = {
        .type = DE_PACKAGE_RECEIVED,
        .data = package_ptr
    };
    
    handle_dexcom_event(ctx, event);
    return;
}



void read_data_from_cc2500(dexcom_ctx_t *ctx)
{
    LOG_INF("GDO0 asserted, which indicates end of packet received");
    ctx->last_interrupt_time = k_uptime_get_32();
    
    // Read offset on which we got this package, and update it for next transmission
    u8_t offset;
    cc2500_read_status_reg(ctx->cc_ctx, CC2500_REG_FREQEST, &offset, NULL);
    update_frequency_offset(ctx->current_channel, offset);

    u8_t rxbytes, status;
    cc2500_read_status_reg(ctx->cc_ctx, CC2500_REG_RXBYTES, &rxbytes, &status);

    // If overflow, ignore it all and retry
    if ((rxbytes & 0x80)) { 
        LOG_INF("Overflow in RX FIFO, ignore data");
    }
    else if ((rxbytes & 0x7F) > 0 && (status & 0x07) > 0) {
        LOG_DBG("RXBYTES is: %02X", rxbytes);   
        u8_t num_bytes_rx = rxbytes & 0x7F; // Mask bit 7, which is overflow
        LOG_DBG("%d bytes received", num_bytes_rx);
        
        cc2500_read_burst(ctx->cc_ctx, CC2500_REG_RXFIFO, ctx->data_buffer, num_bytes_rx, NULL);

        process_rx_data(ctx, num_bytes_rx);
    }    

    return;
}