
#include "delay_tim4.h"

uint32_t prescaler_ms;
uint32_t prescaler_us;

void delay_us(uint16_t value)
{
	TIM4->PSC = prescaler_us;
	TIM4->ARR = value;
	TIM4->CNT = 0;
	TIM4->CR1 = TIM_CR1_CEN;
	while((TIM4->SR & TIM_SR_UIF) == 0){}
	TIM4->SR &= ~TIM_SR_UIF;
}

void delay_ms(uint16_t value)
{
	TIM4->PSC = prescaler_ms;
	TIM4->ARR = value;
	TIM4->CNT = 0;
	TIM4->CR1 = TIM_CR1_CEN;
	while((TIM4->SR & TIM_SR_UIF) == 0){}
	TIM4->SR &= ~TIM_SR_UIF;
}

void delay_tim4_init(void)
{
	prescaler_ms = (uint16_t) ((SystemCoreClock / 2) / 2000) - 1;
	prescaler_us = (uint16_t) ((SystemCoreClock / 2) / 2000000) - 1;
	
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
}
