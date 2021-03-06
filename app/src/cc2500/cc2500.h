#ifndef CC2500_H
#define CC2500_H

#include <zephyr.h>
#include <spi.h>

typedef struct device device_t;
typedef struct gpio_callback gpio_callback_t;

typedef struct spi_buf spi_buf_t;
typedef struct spi_buf_set spi_buf_set_t;
typedef struct spi_config spi_config_t;

typedef enum cc2500_mode {ON, SLEEP, OFF} cc2500_mode_t;

typedef struct cc2500_packet {
    u8_t len;
    u32_t dest_addr;
    u32_t src_addr;
    u8_t port;
    u8_t info;
    u8_t txID;
    u16_t raw;
    u16_t filtered;
    u8_t battery;
    u8_t padding;
    u8_t checksum;
    u8_t RSSI;
    u8_t LQI;
} cc2500_packet_t;

typedef struct cc2500_ctx {
    device_t *gpio_dev;
    device_t *spi_dev;
    spi_config_t spi_conf;
    
    int gdo0_pin;
    int gdo2_pin;
    gpio_callback_t *gdo0_cb;
    gpio_callback_t *gdo2_cb;

    cc2500_mode_t mode;
} cc2500_ctx_t;

bool cc2500_verify_osc_stabilization(cc2500_ctx_t *ctx);

int cc2500_reset(cc2500_ctx_t *ctx);

int cc2500_read_package(cc2500_ctx_t *ctx);

int cc2500_mode_idle(cc2500_ctx_t *ctx);
int cc2500_mode_off(cc2500_ctx_t *ctx);

int cc2500_mode_receive(cc2500_ctx_t *ctx, u8_t channel);

int cc2500_flush_rxfifo(cc2500_ctx_t *ctx);

void update_frequency_offset(u8_t channel, int8_t offset);

#endif /* CC2500_H */