#pragma once
#include <zephyr.h>

typedef struct dexcom_package {
    s64_t timestamp;
    u64_t transmitterId;
    u32_t rawIsig;
    u32_t filIsig;
    u16_t batLevel;
} dexcom_package_t;

typedef struct dexcom_ctx {
    u64_t transmitterId;
    struct k_fifo *package_queue;
} dexcom_ctx_t;

void test_cc2500(dexcom_ctx_t *dex_ctx);