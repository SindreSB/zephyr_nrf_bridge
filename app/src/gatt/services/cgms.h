#pragma once
#include <zephyr/types.h>
#include "../../dexcom/receiver.h"


// Create service
void cgms_init(void);

void cgms_send_all();

void cgms_add_measurement(dexcom_package_t reading);