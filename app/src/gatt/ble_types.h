#pragma once
#include <zephyr/types.h>

typedef struct ble_time {
    u16_t year;
    u8_t month;
    u8_t day;
    u8_t hour;
    u8_t minute;
    u8_t second;
} ble_time_t;

typedef struct ble_meas_rec {
    u16_t filtered;
    u16_t raw;
    u8_t trans_batt;
    ble_time_t time;
    u8_t status;
} ble_meas_rec_t;