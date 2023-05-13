#include <avr/io.h>

typedef struct _EepromData
{
    int MajorVersion;
    int MinorVersion;
    int HeaderLength;
    int TransmitInterval;
    uint8_t crc;
} EepromData;