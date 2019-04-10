#include <zephyr.h>

#define MANT_MAX ((1 << 12 ) -1)

/**
 * @brief Convert a positive integer to an short float
 * 
 * Converts an int32 to a SFLOAT inspired format. 
 * Only works for positive numbers
 * 
 * @param value 
 * @return u16_t 
 */
u16_t intToSfloat(u32_t value)
{
	u8_t exponent = 0;
	while (value > MANT_MAX) {
		value /= 10;
		exponent++;
	}

	return (exponent << 12) | (value & 0x00000FFF);
}