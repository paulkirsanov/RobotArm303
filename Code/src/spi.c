#include "spi.h"

void spi1_init(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	
	GPIOA->AFR[0] |= 0x55500000;
		
	//SS
	GPIOA->MODER |= GPIO_MODER_MODER4_0;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT_4;
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR4;
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR4;
	GPIOA->BSRR |= GPIO_BSRR_BS_4;
	
	//SCK
	GPIOA->MODER |= GPIO_MODER_MODER5_1;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT_5;
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR5;
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR5;
	
	//MISO
	GPIOA->MODER &= ~GPIO_MODER_MODER6_1;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT_6;
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6;
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR6;
	GPIOA->BSRR |= GPIO_BSRR_BS_6;
	
	//MOSI
	GPIOA->MODER |= GPIO_MODER_MODER7_1;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT_7;
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR7;
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR7;
	
	SPI1->CR1 = 0x0000;
	SPI1->CR2 = 0x0000;
	SPI1->CR1 |= SPI_CR1_BR;																						//Baud rate control Fpclk/256
	SPI1->CR1 |= SPI_CR1_CPOL;																					//Clock polarity CPOL = 0
	SPI1->CR1 |= SPI_CR1_CPHA;																					//Clock phase CPHA = 0
	SPI1->CR2 |= ( SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2 );				//Data size 8-bit
	SPI1->CR1 &= ~SPI_CR1_LSBFIRST;																			//Frame format (MSB transmitted first)
	SPI1->CR1 |= SPI_CR1_SSM;																						//Software slave management
	SPI1->CR1 |= SPI_CR1_SSI;																						//Internal slave select
	SPI1->CR1 |= SPI_CR1_MSTR;																					//Master mode
	
	SPI1->CR1 |= SPI_CR1_SPE;																						//Enable SPI1
}

void spi1_write(uint8_t data)
{
	SPI1->DR = data;
	while(!(SPI1->SR & SPI_SR_TXE));
}

uint8_t spi1_read(void)
{
	while(!(SPI1->SR & SPI_SR_RXNE));
	return SPI1->DR;
}
