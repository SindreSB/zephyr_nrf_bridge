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
#include "../cc2500/utils.h"

#include "receiver.h"
#include "utils.h"

LOG_MODULE_REGISTER(dex_receiver);

// Hack so that the interrupt handler gets access to the handler
dexcom_ctx_t* receiver_context;

// Forward declare internal functions
int configure_cc2500(dexcom_ctx_t *ctx);
void handle_dexcom_event(dexcom_ctx_t *ctx, dexcom_event_t event);

void enter_sleep(dexcom_ctx_t *ctx);

void process_rx_data(u8_t *buffer, u8_t length);
void read_data_from_cc2500(dexcom_ctx_t *ctx);
void submit_package_read(device_t *gpiob, gpio_callback_t *cb, u32_t pins);


/**
 * @brief Start and configure the CC2500 for package receiption
 * 
 * 
 * @param ctx The Dexcom receiver context to start
 */
void start_cc2500(dexcom_ctx_t *ctx) {
    // Hack before I can pass context  to interrupt handler
    receiver_context = ctx;

    // First configure the cc2500
    int ret;
    ret = configure_cc2500(ctx);
    if (ret) {
        LOG_ERR("Unable to start CC2500");
        return;
    }

    // Set up callback for package received
    cc2500_register_gdo0_handler(ctx->cc_ctx, submit_package_read, GPIO_INT_EDGE | GPIO_INT_ACTIVE_HIGH);
    
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
    LOG_INF("Asserting XTAL is stable");
    if(!cc2500_verify_osc_stabilization(receiver_context->cc_ctx)) {
        LOG_INF("CC2500 didn't become ready in time");
        return -1;
    }
    
    u8_t value, status;

    LOG_INF("Reading off values to ensure CC2500 is working");
    cc2500_read_status_reg(receiver_context->cc_ctx, CC2500_REG_PARTNUM, &value, &status);
    LOG_INF("Partnum: 0x%02X, should be 0x80", value);

    cc2500_read_status_reg(receiver_context->cc_ctx, CC2500_REG_VERSION, &value, &status);
    LOG_INF("Version: 0x%02X, should be 0x03", value);

    // Write required register values
    LOG_INF("Writing Dexcom config to device");
    cc2500_write_register(receiver_context->cc_ctx, CC2500_REG_PATABLE,     0x00, NULL);
    cc2500_write_register(receiver_context->cc_ctx, CC2500_REG_IOCFG0,	    0x01, NULL);
    cc2500_write_register(receiver_context->cc_ctx, CC2500_REG_PKTLEN,	    0xFF, NULL);
    cc2500_write_register(receiver_context->cc_ctx, CC2500_REG_PKTCTRL1,	0x04, NULL);
    cc2500_write_register(receiver_context->cc_ctx, CC2500_REG_PKTCTRL0,	0x05, NULL);
    cc2500_write_register(receiver_context->cc_ctx, CC2500_REG_ADDR,	    0x00, NULL);
    cc2500_write_register(receiver_context->cc_ctx, CC2500_REG_CHANNR,	    0x00, NULL);

    cc2500_write_register(receiver_context->cc_ctx, CC2500_REG_FSCTRL1,	    0x0F, NULL);
    cc2500_write_register(receiver_context->cc_ctx, CC2500_REG_FSCTRL0,	    0x00, NULL);
    
    cc2500_write_register(receiver_context->cc_ctx, CC2500_REG_FREQ2,	    0x5D, NULL);
    cc2500_write_register(receiver_context->cc_ctx, CC2500_REG_FREQ1,	    0x44, NULL);
    cc2500_write_register(receiver_context->cc_ctx, CC2500_REG_FREQ0,	    0xEB, NULL);
    
    cc2500_write_register(receiver_context->cc_ctx, CC2500_REG_FREND1,	    0xB6, NULL);
    cc2500_write_register(receiver_context->cc_ctx, CC2500_REG_FREND0,	    0x10, NULL);
    
    cc2500_write_register(receiver_context->cc_ctx, CC2500_REG_MDMCFG4,	    0x7A, NULL);
    cc2500_write_register(receiver_context->cc_ctx, CC2500_REG_MDMCFG3,	    0xF8, NULL);
    cc2500_write_register(receiver_context->cc_ctx, CC2500_REG_MDMCFG2,	    0x73, NULL);
    cc2500_write_register(receiver_context->cc_ctx, CC2500_REG_MDMCFG1,	    0x23, NULL);
    cc2500_write_register(receiver_context->cc_ctx, CC2500_REG_MDMCFG0,	    0x3B, NULL);
    
    cc2500_write_register(receiver_context->cc_ctx, CC2500_REG_DEVIATN,	    0x40, NULL);
    
    cc2500_write_register(receiver_context->cc_ctx, CC2500_REG_MCSM2,	    0x07, NULL);
    cc2500_write_register(receiver_context->cc_ctx, CC2500_REG_MCSM1, 	    0x30, NULL);
    cc2500_write_register(receiver_context->cc_ctx, CC2500_REG_MCSM0,	    0x18, NULL);
    
    cc2500_write_register(receiver_context->cc_ctx, CC2500_REG_FOCCFG,	    0x16, NULL);
    
    cc2500_write_register(receiver_context->cc_ctx, CC2500_REG_FSCAL3,	    0xA9, NULL);
    cc2500_write_register(receiver_context->cc_ctx, CC2500_REG_FSCAL2,	    0x0A, NULL);
    cc2500_write_register(receiver_context->cc_ctx, CC2500_REG_FSCAL1,	    0x00, NULL);
    cc2500_write_register(receiver_context->cc_ctx, CC2500_REG_FSCAL0,	    0x11, NULL);
    
    cc2500_write_register(receiver_context->cc_ctx, CC2500_REG_AGCCTRL2,    0x03, NULL);
    cc2500_write_register(receiver_context->cc_ctx, CC2500_REG_AGCCTRL1,    0x00, NULL);
    cc2500_write_register(receiver_context->cc_ctx, CC2500_REG_AGCCTRL0,    0x91, NULL);
    
    cc2500_write_register(receiver_context->cc_ctx, CC2500_REG_TEST2,	    0x81, NULL);
    cc2500_write_register(receiver_context->cc_ctx, CC2500_REG_TEST1,	    0x35, NULL);
    cc2500_write_register(receiver_context->cc_ctx, CC2500_REG_TEST0,	    0x0B, NULL);
    
    cc2500_write_register(receiver_context->cc_ctx, CC2500_REG_FOCCFG,	    0x0A, NULL);
    
    cc2500_write_register(receiver_context->cc_ctx, CC2500_REG_BSCFG,	    0x6C, NULL);

    LOG_INF("All setup completed");
    return 0;
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
        // Package received -> put in package fifo
        k_fifo_alloc_put(receiver_context->package_queue, (dexcom_package_t *) event.data);

        // Set last received

        // Go to sleep

        // For now, go back into receive
        cc2500_mode_receive(ctx->cc_ctx, 0);

        return;
    }
    else if (event.type == DE_PACKAGE_INVALID) 
    {
        // For now, go back into receive
        cc2500_mode_receive(ctx->cc_ctx, 0);

        return;

        if (ctx->current_channel == 3) {
            // We have not gotten a single success
            // If we have gotten a good package before, do long sleep, otherwise 
            // go back into receive on channel 0
            if (ctx->last_package_received == DE_NO_PACKAGE)
            {
                cc2500_mode_receive(ctx->cc_ctx, 0);
            }
            else 
            {

            }
        }
    }
    else if (event.type == DE_SLEEP_TIMEOUT)
    {
        // We need to configure CC again
        configure_cc2500(ctx);

        // Go into receive
        ctx->current_channel = 0;
        cc2500_mode_receive(ctx->cc_ctx, 0);

        return;
    }
    else if (event.type == DE_CHANNEL_TIMEOUT) 
    {
        // Go to next channel
        ctx->current_channel++;
        // Go into receive
        cc2500_mode_receive(ctx->cc_ctx, ctx->current_channel);

        return;
    }
}


void enter_sleep(dexcom_ctx_t *ctx)
{
    // Send SXOFF
    cc2500_mode_off(ctx->cc_ctx);

    // Calculate sleeptime
    

    // Set timer
}


void process_rx_data(u8_t *buffer, u8_t length) 
{
    u8_t current_byte = 0;
    while(current_byte < length){
        // Check first byte, which is package length and that there are enough bytes left
        if (buffer[current_byte] != 18 || (current_byte + 18) >= length) {
            LOG_INF("Package length issue");
            current_byte += buffer[current_byte]; // Add the assumed package length we now ignored
            continue;
        }

        // TODO: Check package CRC

        // Check payload CRC
        u8_t crc = compute_crc8_simple(0x2F, buffer + current_byte + 12, 7);
        if (crc != 0x00) {
            LOG_INF("CRC failed for payload");
            current_byte += 18;
            continue;
        } 
        else 
        {
            LOG_INF("CRC validated payload");
        }

        dexcom_package_t *package_ptr = k_malloc(sizeof(dexcom_package_t));
        package_ptr->timestamp = k_uptime_get();
        package_ptr->transmitterId = 0;
        package_ptr->rawIsig = extractISIG(buffer + current_byte + 12);
        package_ptr->filIsig = extractISIG(buffer + current_byte + 14);
        package_ptr->batLevel = 0;

        // Ignore rest of buffer
        current_byte = length;

        dexcom_event_t event = {
            .type = DE_PACKAGE_RECEIVED,
            .data = package_ptr
        };
        
        handle_dexcom_event(receiver_context, event);
        return;
    }
    
    dexcom_event_t event = {
        .type = DE_PACKAGE_INVALID
    };

    handle_dexcom_event(receiver_context, event);  
    return;
}

void package_received_work_handler(struct k_work *item) {
    read_data_from_cc2500(receiver_context);
}


void read_data_from_cc2500(dexcom_ctx_t *ctx)
{
    LOG_INF("GDO0 asserted, which indicates end of packet received");

    cc2500_mode_idle(ctx->cc_ctx);
    
    u8_t rxbytes, status;
    cc2500_read_status_reg(ctx->cc_ctx, CC2500_REG_RXBYTES, &rxbytes, &status);

    LOG_INF("Status is: %02X, rxbytes is: %02X", status, rxbytes);

    int success;
    // If overflow, ignore it all and retry
    if ((rxbytes & 0x80)) { 
        LOG_INF("Overflow in RX FIFO, ignore data");
        success = 0;
    }
    else if ((rxbytes & 0x7F) > 0 && (status & 0x07) > 0) {
        LOG_INF("RXBYTES is: %02X", rxbytes);   
        u8_t num_bytes_rx = rxbytes & 0x7F; // Mask bit 7, which is overflow
        LOG_INF("%d bytes received", num_bytes_rx);
        
        cc2500_read_burst(ctx->cc_ctx, CC2500_REG_RXFIFO, ctx->data_buffer, num_bytes_rx, NULL);

        process_rx_data(ctx->data_buffer, num_bytes_rx);
    }    

    return;
}

K_WORK_DEFINE(work, package_received_work_handler);
void submit_package_read(struct device *gpiob, struct gpio_callback *cb,
		    u32_t pins)
{
    LOG_INF("Submitting read package job");
    k_work_submit(&work);
}