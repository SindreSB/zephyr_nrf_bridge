#pragma once
#include <zephyr/types.h>
#include "../../dexcom/receiver.h"


// Create service
void cgms_init(void);

void cgms_failed_reading();
void cgms_add_measurement(dexcom_package_t reading);