/**
 * 
 * Represents a "HAL" for communicating with the CC2500.
 * 
 **/
#include <zephyr.h>
#include <misc/printk.h>

#ifndef CC2500_SPI_H
#define CC2500_SPI_H

int cc2500_send_strobe(u8_t strobe, u8_t *result, u8_t *status);

int cc2500_write_register(u8_t address, u8_t value, u8_t *status);
int cc2500_read_register(u8_t address, u8_t *value, u8_t *status);

int cc2500_read_status_reg(u8_t address, u8_t *value, u8_t *status);
int cc2500_read_burst(u8_t address, u8_t *buffer, int count);

int cc2500_read_status_byte(u8_t * status);

#endif /* CC2500_SPI_H */