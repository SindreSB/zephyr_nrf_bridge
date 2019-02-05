/**
 * 
 * Represents a "HAL" for communicating with the CC2500.
 * 
 **/
#ifndef CC2500_SPI_H
#define CC2500_SPI_H

#include <zephyr.h>
#include <gpio.h>
#include <misc/printk.h>

#include "cc2500.h"

int cc2500_send_strobe(cc2500_ctx_t *ctx, u8_t strobe, u8_t *status);

int cc2500_write_register(cc2500_ctx_t *ctx, u8_t address, u8_t value, u8_t *status);
int cc2500_read_register(cc2500_ctx_t *ctx, u8_t address, u8_t *value, u8_t *status);

int cc2500_read_status_reg(cc2500_ctx_t *ctx, u8_t address, u8_t *value, u8_t *status);
int cc2500_read_burst(cc2500_ctx_t *ctx, u8_t address, u8_t *buffer, int count, u8_t *status);

int cc2500_read_status_byte(cc2500_ctx_t *ctx, u8_t *status);


int cc2500_register_gdo0_handler(cc2500_ctx_t *ctx, gpio_callback_handler_t handler, u8_t edge);
int cc2500_register_gdo2_handler(cc2500_ctx_t *ctx, gpio_callback_handler_t handler, u8_t edge);

#endif /* CC2500_SPI_H */