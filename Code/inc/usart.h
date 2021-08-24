#ifndef __usart_h
#define __usart_h

#include "stm32f30x.h"

void usart1_init(void);
void usart2_init(void);

void usart_send(USART_TypeDef* USARTx, char data);
void usart_send_data(USART_TypeDef* USARTx, uint8_t data);
void usart_send_string(USART_TypeDef* USARTx, char* str);

#endif
