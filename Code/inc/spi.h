#ifndef __spi_h
#define __spi_h

#include "stm32f30x.h"

#define CS_HIGH()									GPIOA->BSRR |= GPIO_BSRR_BS_4;
#define CS_LOW()									GPIOA->BRR |= GPIO_BRR_BR_4;

void spi1_init(void);
void spi_write(uint8_t data);
uint8_t spi_read(void);

#endif
