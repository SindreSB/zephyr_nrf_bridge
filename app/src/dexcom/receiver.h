#pragma once
#include <zephyr.h>
#include "../cc2500/cc2500.h"

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
    u64_t last_package_received;
    u8_t current_channel;

    u8_t *data_buffer;
    u8_t bytes_in_buffer;

    cc2500_ctx_t * cc_ctx;
} dexcom_ctx_t;

void start_cc2500(dexcom_ctx_t *dex_ctx);