#include "timer.h"

void timer2_init(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	
	TIM2->PSC = 35999;
	TIM2->ARR = 9;
	TIM2->CNT = 0;
	TIM2->EGR = TIM_EGR_UG;
	TIM2->DIER |= TIM_DIER_UIE;
	TIM2->CR1 = TIM_CR1_CEN;
	NVIC_SetPriority(TIM2_IRQn, 2);
}

void timer3_init(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	
	TIM3->PSC = 35999;
	TIM3->ARR = 49;
	TIM3->CNT = 0;
	TIM3->EGR = TIM_EGR_UG;
	TIM3->DIER |= TIM_DIER_UIE;
	TIM3->CR1 = TIM_CR1_CEN;	
	NVIC_SetPriority(TIM3_IRQn, 1);
}

void timer4_pwm_init(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	
	GPIOB->MODER &= ~GPIO_MODER_MODER8;														//Channel 3
	GPIOB->MODER |= GPIO_MODER_MODER8_1;													//Alternate function mode
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT_8;														//Output push-pull (reset state)
	GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8;											//High speed
	
	GPIOB->MODER &= ~GPIO_MODER_MODER9;														//Channel 4
	GPIOB->MODER |= GPIO_MODER_MODER9_1;													//Alternate function mode
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT_9;														//Output push-pull (reset state)
	GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR9;											//High speed
	
	GPIOB->AFR[1] |= 0x00000022;
	
	TIM4->CCER |= TIM_CCER_CC3E;
	TIM4->CCER |= TIM_CCER_CC4E;
	
	TIM4->BDTR |= TIM_BDTR_MOE;
	
	TIM4->CCMR2 &= ~TIM_CCMR2_CC3S;
	TIM4->CCMR2 &= ~TIM_CCMR2_OC3M;
	
	TIM4->CCMR2 &= ~TIM_CCMR2_CC4S;
	TIM4->CCMR2 &= ~TIM_CCMR2_OC4M;
	
	TIM4->CCMR2 |= (TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2);					//PWM mode 1
	TIM4->CCMR2 |= (TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2);					//PWM mode 1
	
	TIM4->CR1 &= ~TIM_CR1_DIR;
	TIM4->CR1 &= ~TIM_CR1_CMS;
	
	TIM4->PSC = 21;																							//36
	TIM4->ARR = 65450;																					//10000
	
	TIM4->CR1 = TIM_CR1_CEN;
}
