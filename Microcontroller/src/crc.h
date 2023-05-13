#include <avr/io.h>
#include <stdlib.h>
uint8_t crc_8(const unsigned char *input_str, size_t num_bytes);
uint8_t update_crc_8(unsigned char crc, unsigned char val);