#include "io.h"

#define		cIO_COUNT		(18)

const tGPIO_Line IOs[cIO_COUNT] = 	 {{ GPIOB, 12, OUT, MEDIUM_SPEED, PP, PU, HIGH },  				// Dir1
																			{ GPIOB, 14, OUT, MEDIUM_SPEED, PP, PU, HIGH },	 				// En1
																			{ GPIOB, 13, OUT, MEDIUM_SPEED, PP, PU, HIGH },        	// Step1
																			{ GPIOB, 1, OUT, MEDIUM_SPEED, PP, PU, HIGH },					// MS10
																			{ GPIOB, 10, OUT, MEDIUM_SPEED, PP, PU, HIGH },					// MS11
																			{ GPIOB, 11, OUT, MEDIUM_SPEED, PP, PU, HIGH },					// MS12
																			{ GPIOF, 9, IN, PU, LOW },															// io_SNS1
																			
																			{ GPIOA, 1, OUT, MEDIUM_SPEED, PP, PU, HIGH },					// Dir2
																			{ GPIOA, 2, OUT, MEDIUM_SPEED, PP, PU, HIGH },					// En2
																			{ GPIOA, 0, OUT, MEDIUM_SPEED, PP, PU, HIGH },					// Step2
																			{ GPIOA, 3, OUT, MEDIUM_SPEED, PP, PU, HIGH },					// MS20
																			{ GPIOB, 0, OUT, MEDIUM_SPEED, PP, PU, HIGH },					// MS21
																			{ GPIOB, 2, OUT, MEDIUM_SPEED, PP, PU, HIGH },					// MS22
																			{ GPIOF, 10, IN, PU, LOW },															// io_SNS2
																			
																			{ GPIOD, 1, OUT, MEDIUM_SPEED, PP, PU, HIGH },					// Dir3
																			{ GPIOD, 2, OUT, MEDIUM_SPEED, PP, PU, HIGH },					// En3
																			{ GPIOD, 3, OUT, MEDIUM_SPEED, PP, PU, HIGH },					// Step3
																			{ GPIOD, 4, IN, PU, LOW }																// io_SNS3
};

void IO_SetLine(tIOLine Line, bool State)
{
	if (State)
	{
		IOs[Line].GPIOx->BSRR = 1 << (IOs[Line].GPIO_Pin);
	}
	else
	{
		IOs[Line].GPIOx->BRR = 1 << (IOs[Line].GPIO_Pin);
	}
}

bool IO_GetLine(tIOLine Line)
{
	if (Line <= cIO_COUNT)
		return ((IOs[Line].GPIOx->IDR) & (1 << IOs[Line].GPIO_Pin));
	else
		return false;
}

void IO_ConfigLine(tIOLine Line, uint8_t Mode, uint8_t Speed, uint8_t Type, uint8_t pupdr, tIOState State)
{
	IOs[Line].GPIOx->MODER &= ~(1 << IOs[Line].GPIO_Pin * 2);								// Стираем биты
	IOs[Line].GPIOx->MODER |=  Mode << (IOs[Line].GPIO_Pin * 2);						// Вносим нашу битмаску, задвинув ее на нужное место
	
	IOs[Line].GPIOx->OTYPER &= ~(1 << IOs[Line].GPIO_Pin * 2);							// Стираем биты
	IOs[Line].GPIOx->OTYPER |= Type << (IOs[Line].GPIO_Pin * 2);						// Прописываем OTYPER
	
	IOs[Line].GPIOx->OSPEEDR &= ~(1 << IOs[Line].GPIO_Pin * 2);							// Стираем биты
	IOs[Line].GPIOx->OSPEEDR |= Speed << (IOs[Line].GPIO_Pin * 2);					// Прописываем OTYPER
	
	IOs[Line].GPIOx->PUPDR &= ~(1 << IOs[Line].GPIO_Pin * 2);								// Стираем биты
	IOs[Line].GPIOx->PUPDR |= pupdr << (IOs[Line].GPIO_Pin * 2);						// Прописываем PUPDR

	IOs[Line].GPIOx->ODR &= ~(1 << IOs[Line].GPIO_Pin);											// Стираем биты
	IOs[Line].GPIOx->ODR |= State << IOs[Line].GPIO_Pin;										// Прописываем ODR
}

void IO_Init(void)
{
	int Line = 0;
		
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	RCC->AHBENR |= RCC_AHBENR_GPIODEN;
	RCC->AHBENR |= RCC_AHBENR_GPIOEEN;
	RCC->AHBENR |= RCC_AHBENR_GPIOFEN;

	for (Line = 0; Line < cIO_COUNT; Line++)
	{
		IO_ConfigLine(Line, IOs[Line].MODE, IOs[Line].SPEED, IOs[Line].TYPE, IOs[Line].PUPDR, IOs[Line].DefState);
	}
}
