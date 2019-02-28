#pragma once
#include <zephyr.h>
#include "../cc2500/cc2500.h"

#define DE_NO_PACKAGE -1

typedef enum DE_EVENT {
    DE_PACKAGE_RECEIVED,
    DE_PACKAGE_INVALID,
    DE_SLEEP_TIMEOUT,
    DE_CHANNEL_TIMEOUT
} DE_EVENT_T;

typedef struct dexcom_package {
    s64_t timestamp;
    u64_t transmitterId;
    u32_t rawIsig;
    u32_t filIsig;
    u16_t batLevel;
} dexcom_package_t;

typedef struct dexcom_event {
    DE_EVENT_T type;
    
    void *data;
} dexcom_event_t;

typedef struct dexcom_ctx {
    u64_t transmitterId;
    struct k_fifo *package_queue;
    
    u8_t current_channel;
    s64_t last_package_received;


    u8_t *data_buffer;
    u8_t bytes_in_buffer;

    cc2500_ctx_t * cc_ctx;
    gpio_callback_t gdo0_interrupt_handler;
} dexcom_ctx_t;

void start_cc2500(dexcom_ctx_t *dex_ctx);

/**
 * @brief 
 * 
 */
# define DEXCOM_RECEIVER(NAME)                      \
    static gpio_callback_t NAME##_gdo0_cb;          \
    static gpio_callback_t NAME##_gdo2_cb;          \
    cc2500_ctx_t NAME##_cc_context = {              \
        .gdo0_cb = &NAME##_gdo0_cb,                 \
        .gdo2_cb = &NAME##_gdo2_cb,                 \
    };                                              \
    K_FIFO_DEFINE( NAME##_pkt_fifo);                \
    u8_t NAME##_data_buffer[64];                    \
    dexcom_ctx_t NAME = {                           \
        .last_package_received = -1,                \
        .package_queue = &NAME##_pkt_fifo,          \
        .data_buffer = NAME##_data_buffer,          \
        .cc_ctx = &NAME##_cc_context,               \
    }
