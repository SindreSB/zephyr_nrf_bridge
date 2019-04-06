/**
 * @file rdb.h
 * @author your name (you@domain.com)
 * @brief Record database implemented as a circular buffer
 * @version 0.1
 * @date 2019-04-06
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once
#include <zephyr/types.h>

/**
 * Implements the Records store as a circular buffer, if full,
 * records old recoreds will be discarded
 * 
 * TODO: eventually, size of store should be set in Kconfig
 * 
 */

typedef struct meas_record {
    u16_t filtered;
    u16_t raw;
    u8_t trans_batt;
    u32_t timestamp;
} meas_record_t;


/**
 * @brief Get a pointer to next re
 * 
 * @param rec_ptr Pointer where new record can be stored
 * @return error code 
 */
 s32_t rdb_new(meas_record_t** rec_ptr);

/**
 * @brief Return pointer to record number i
 * 
 * @param index 
 * @return meas_record_t* 
 */
s32_t rdb_get(u32_t index, meas_record_t** rec_ptr);

/**
 * @brief Deletes all entried up to index inclusive
 * 
 * @param index 
 * @return u32_t status
 */
s32_t rdb_delete(u32_t index);

/**
 * @brief Return number of records stored
 * 
 * @return size_t 
 */

u32_t rdb_size();

/**
 * @brief Inits or reset the store
 * 
 * @return u32_t 
 */
s32_t rdb_init();