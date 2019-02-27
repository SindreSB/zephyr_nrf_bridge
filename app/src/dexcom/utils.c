#include <zephyr.h>

/**
 * @brief Compute the CRC8 of an array of bytes
 * 
 * Can be used to calculate CRC, or by including CRC 
 * as last byte, can validate previously generated CRC.
 * 
 * @param generator The polynomial to be used
 * @param bytes A pointer to a byte array
 * @param length Number of bytes in the array to use in calculation
 * @return u8_t The final CRC value
 */
u8_t compute_crc8_simple(u8_t generator, u8_t *bytes, u8_t length)
{
    u8_t crc = 0;

    for (int i = 0; i < length; i++)
    {
        crc ^= bytes[i];

        for (int j = 0; j < 8; j++)
        {
            if ((crc & 0x80) != 0)
            {
                crc = (u8_t)((crc << 1) ^ generator);
            }
            else
            {
                crc <<= 1;
            }
        }
    }

    return crc;
}

/**
 * @brief Compute 2^exp
 * 
 * @param exp The exponent
 * @return uint32_t The result of the exponentiation
 */
uint32_t powerOfTwo(uint32_t exp)
{
    int i = 0; /* Variable used in loop counter */
    int number = 1;

    while (i < exp)
    {
        number *= 2;
        i++;
    }

    return (number);
}

/*
Copied from 
https://stackoverflow.com/a/12357926/
*/
u16_t reverse16(u16_t v)
{
    v = ((v >> 1) & 0x5555) | ((v & 0x5555) << 1); /* swap odd/even bits */
    v = ((v >> 2) & 0x3333) | ((v & 0x3333) << 2); /* swap bit pairs */
    v = ((v >> 4) & 0x0F0F) | ((v & 0x0F0F) << 4); /* swap nibbles */
    v = ((v >> 8) & 0x00FF) | ((v & 0x00FF) << 8); /* swap bytes */
    return v;
}

/*
Copied from
https://stackoverflow.com/a/2602885
*/
u8_t reverse8(u8_t b)
{
    b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
    b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
    b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
    return b;
}

/*
  This converts a float represented using 16 bits, with
  the first 3 bits as exponent, and the rest as remainder
*/
float convertFloat(u16_t value)
{
    uint16_t exp = value >> 13;    // Left shift 13 times to extract the exponent( top 3 bits)
    uint16_t man = value & 0x1FFF; // Clear first three bits, and interpet rest as int

    float result = powerOfTwo(exp) * man * 2; // (2 ^ exp) * mantissa.

    return result;
}

float isigToMmol(float value)
{
    float mgdl = (value / 1000.0);
    return (1.0 / 18.0) * mgdl;
}

/**
 * @brief Read two bytes from a Dexcom package and extract ISIG value
 * 
 * Will suppress the decimals from the float value
 *  
 * @param firstByte Pointer to first (of two) bytes 
 * @return u16_t Value with the resulting ISIG value
 */
u16_t extractISIG(u8_t *firstByte)
{
    u16_t raw = firstByte[0] << 8 | firstByte[1];
    return convertFloat(reverse16(raw));
}