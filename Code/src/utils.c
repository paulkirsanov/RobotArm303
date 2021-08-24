#include "utils.h"

void button_init(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	GPIOC->MODER &= ~GPIO_MODER_MODER0;
	GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR0;
	
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	GPIOA->MODER &= ~GPIO_MODER_MODER0;
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR0;
}

uint16_t convertFrom8to16(uint8_t dataFirst, uint8_t dataSecond)
{
	return dataFirst << 8 | dataSecond;
}

uint32_t convertFrom8to32(uint8_t *buffer)
{
	return (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
}
