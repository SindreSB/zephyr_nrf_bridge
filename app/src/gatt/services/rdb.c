#include <zephyr/types.h>

#include "rdb.h"

#define RDB_STORE_CAPACITY 100
// Backing store

static meas_record_t rd_store[RDB_STORE_CAPACITY];
static u32_t head = 0; // Points to first unused record, ie last valid record is head - 1
static u32_t  tail = 0;

s32_t rdb_new(meas_record_t** rec_ptr) 
{
    *rec_ptr = &(rd_store[head]);
    
    head = (head + 1) % RDB_STORE_CAPACITY;

    return 0;
}

s32_t rdb_get(u32_t index, meas_record_t** rec_ptr) 
{
    if (tail + index < rdb_size()) {
        *rec_ptr = &(rd_store[tail + index % RDB_STORE_CAPACITY]);
        return 0;
    }
    return -1;
}

s32_t rdb_pop() {
    if (rdb_size() <= 0) {
        return -1;
    }

    tail = (tail + 1) % RDB_STORE_CAPACITY;

    return 0;
}

s32_t rdb_get_last(meas_record_t** rec_ptr) {
    if (rdb_size() <= 0) {
        return -1;
    }

    *rec_ptr = &rd_store[tail];

    return 0;
}


s32_t rdb_delete(u32_t index)
{
    if (tail + index < rdb_size()) {
        tail = (tail + index + 1) % RDB_STORE_CAPACITY;
    } else {
        rdb_init();
    }

    return 0;
}

u32_t rdb_size()
{
    if (head == tail) return 0;
    if (head > tail) return head - tail;
    else return head - tail + RDB_STORE_CAPACITY;
}

s32_t rdb_init()
{
    head = 0;
    tail = 0;

    return 0;
}