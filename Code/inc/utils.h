#ifndef __utils_h
#define __utils_h

#include "stm32f30x.h"
#include "utils.h"

void button_init(void);
uint16_t convertFrom8to16(uint8_t dataFirst, uint8_t dataSecond);
uint32_t convertFrom8to32(uint8_t *buffer);

#endif
