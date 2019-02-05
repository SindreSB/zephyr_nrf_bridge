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
typedef void (*InterruptHandler)(void);

typedef struct cc2500_config {
    u8_t IOCFG1  ;
    u8_t IOCFG0  ;
    u8_t FIFOTHR ;
    u8_t SYNC1   ;
    u8_t SYNC0   ;
    u8_t PKTLEN  ;
    u8_t PKTCTRL1;
    u8_t PKTCTRL0;
    u8_t ADDR    ;
    u8_t CHANNR  ;
    u8_t FSCTRL1 ;
    u8_t FSCTRL0 ;
    u8_t FREQ2   ;
    u8_t FREQ1   ;
    u8_t FREQ0   ;
    u8_t MDMCFG4 ;
    u8_t MDMCFG3 ;
    u8_t MDMCFG2 ;
    u8_t MDMCFG1 ;
    u8_t MDMCFG0 ;
    u8_t DEVIATN ;
    u8_t MCSM2   ;
    u8_t MCSM1   ;
    u8_t MCSM0   ;
    u8_t FOCCFG  ;
    u8_t BSCFG   ;
    u8_t AGCCTRL2;
    u8_t AGCCTRL1;
    u8_t AGCCTRL0;
    u8_t WOREVT1 ;
    u8_t WOREVT0 ;
    u8_t WORCTRL ;
    u8_t FREND1  ;
    u8_t FREND0  ;
    u8_t FSCAL3  ;
    u8_t FSCAL2  ;
    u8_t FSCAL1  ;
    u8_t FSCAL0  ;
    u8_t RCCTRL1 ;
    u8_t RCCTRL0 ;
    u8_t FSTEST  ;
    u8_t PTEST   ;
    u8_t AGCTEST ;
    u8_t TEST2   ;
    u8_t TEST1   ;
    u8_t TEST0   ;
} cc2500_config_t;

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

int cc2500_configure(cc2500_ctx_t *ctx, cc2500_config_t *config);

int cc2500_reset(cc2500_ctx_t *ctx);

int cc2500_read_package(cc2500_ctx_t *ctx);

int cc2500_mode_idle(cc2500_ctx_t *ctx);

int cc2500_mode_receive(cc2500_ctx_t *ctx, u8_t channel);

int cc2500_flush_rxfifo(cc2500_ctx_t *ctx);


#endif /* CC2500_H */