#ifndef __crc_h
#define __crc_h

#include "stm32f30x.h"

void crc32_init(void);
uint32_t crc_calc(uint32_t pBuffer[], uint32_t BufferLength);

#endif
