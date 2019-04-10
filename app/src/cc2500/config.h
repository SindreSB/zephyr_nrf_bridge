/**
 * @file config.h
 * @author your name (you@domain.com)
 * @brief Contains for hardware used to connect to CC2500
 * @version 0.1
 * @date 2019-04-10
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include <autoconf.h>

#define GPIO_OUT_DRV_NAME CONFIG_CC2500_GPIO

// Correct pins must be configured in DTS
#define SPI_DRV_NAME CONFIG_CC2500_SPI

#define SPI_CS_PIN CONFIG_CC2500_CS_PIN
#define GDO0_PIN CONFIG_CC2500_GDO0_PIN
#define GDO2_PIN CONFIG_CC2500_GDO2_PIN