#include <zephyr.h>


u32_t powerOfTwo(u32_t exp);
u8_t compute_crc8_simple(u8_t generator, u8_t* bytes, u8_t length);

u16_t reverse16(u16_t v);
u8_t reverse8(u8_t b);

u16_t extractISIG(u8_t* firstByte);
float convertFloat(u16_t value);
float isigToMmol(float value);



