#pragma once
#include <zephyr.h>
#include <gpio.h>
#include "../cc2500/cc2500.h"

#define DE_TIME_BETWEEN_TRANS (1000 * 60 * 5)           // 1 min
#define DE_TIME_BETWEEN_CHANNEL (500)                   // 500 ms
#define DE_TIME_ASSUME_MISSED (10 * 1000)               // 10 sec

#define DE_NO_PACKAGE -1

typedef enum DE_EVENT {
    DE_PACKAGE_RECEIVED,
    DE_PACKAGE_INVALID,
    DE_SLEEP_TIMEOUT,
    DE_CHANNEL_TIMEOUT
} DE_EVENT_T;

typedef struct dexcom_package {
    u32_t timestamp;
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
    
    u8_t long_sleep;
    struct k_timer *timeout_timer;

    u8_t current_channel;
    u32_t last_package_received;
    u32_t last_interrupt_time;
    u8_t windows_missed;


    u8_t *data_buffer;
    u8_t bytes_in_buffer;

    cc2500_ctx_t * cc_ctx;
    gpio_callback_handler_t gdo0_interrupt_handler;
} dexcom_ctx_t;

void start_cc2500(dexcom_ctx_t *dex_ctx);

// Forward declare internal functions
int configure_cc2500(dexcom_ctx_t *ctx);
void handle_dexcom_event(dexcom_ctx_t *ctx, dexcom_event_t event);

void enter_sleep(dexcom_ctx_t *ctx);

void process_rx_data(dexcom_ctx_t *ctx, u8_t length);
void read_data_from_cc2500(dexcom_ctx_t *ctx);
void submit_package_read(device_t *gpiob, gpio_callback_t *cb, u32_t pins);


#define HANDLE_PACKAGE_INVALID(ctx)\
    dexcom_event_t event = {.type = DE_PACKAGE_INVALID}; \
    handle_dexcom_event(ctx, event)


#define DEXCOM_INTERRUPT_HANDLER(NAME)                                          \
    void package_received_work_handler(struct k_work *item) {                   \
        read_data_from_cc2500(&NAME);                                           \
    }                                                                           \
    K_WORK_DEFINE(NAME##_interrupt_work, package_received_work_handler);        \
    void NAME##_int_handler(struct device *gpiob, struct gpio_callback *cb,     \
                u32_t pins)                                                     \
    {                                                                           \
        k_work_submit(&NAME##_interrupt_work);                                  \
    }



#define DEXCOM_RECEIVER_TIMER(NAME) \
    void NAME##_timeout_handler(struct k_work *dummy) {                         \
        dexcom_event_t event = {                                                \
            .type = NAME.long_sleep ? DE_SLEEP_TIMEOUT : DE_CHANNEL_TIMEOUT,    \
            .data = NULL,                                                       \
        };                                                                      \
        handle_dexcom_event(&NAME, event);                                      \
    };                                                                          \
    K_WORK_DEFINE(NAME##_timeout_work, NAME##_timeout_handler);                 \
    void NAME##_timer_handler(struct k_timer *dummy)                            \
    {                                                                           \
        k_work_submit(&NAME##_timeout_work);                                     \
    };                                                                          \
    K_TIMER_DEFINE(NAME##_timer, NAME##_timer_handler, NULL)            \

/**
 * @brief 
 * 
 */
#define DEXCOM_RECEIVER(NAME)                                                  \
    static gpio_callback_t NAME##_gdo0_cb;                                      \
    static gpio_callback_t NAME##_gdo2_cb;                                      \
    cc2500_ctx_t NAME##_cc_context = {                                          \
        .gdo0_cb = &NAME##_gdo0_cb,                                             \
        .gdo2_cb = &NAME##_gdo2_cb,                                             \
    };                                                                          \
    K_FIFO_DEFINE( NAME##_pkt_fifo);                                            \
    u8_t NAME##_data_buffer[64];                                                \
    dexcom_ctx_t NAME = {                                                       \
        .last_package_received = -1,                                            \
        .package_queue = &NAME##_pkt_fifo,                                      \
        .data_buffer = NAME##_data_buffer,                                      \
        .cc_ctx = &NAME##_cc_context,                                           \
        .long_sleep = 0,                                                        \
    }
    

