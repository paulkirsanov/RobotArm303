#ifndef __io_h
#define __io_h

#include "stm32f30x.h"
#include <stdbool.h>
#include <stdint.h>

#define LOW_SPEED						(0x00)
#define MEDIUM_SPEED  			(0x01)
#define HIGH_SPEED					(0x03)

#define HIZ									(0x00)
#define PU									(0x01)
#define PD									(0x02)

#define IN									(0x00)
#define OUT									(0x01)
#define AF									(0x02)
#define ADC									(0x03)

#define PP									(0x00)
#define OD									(0x20)

typedef enum
{
	io_DIR1 = 0,
	io_EN1,
	io_STEP1,
	io_MS10,
	io_MS11,
	io_MS12,
	io_SNS1,
	io_DIR2,
	io_EN2,
	io_STEP2,
	io_MS20,
	io_MS21,
	io_MS22,
	io_SNS2,
	io_DIR3,
	io_EN3,
	io_STEP3,
	io_SNS3,
} tIOLine;

typedef enum
{
	OFF = 0,
	ON = 1,
	LOW = 0,
	HIGH = 1
} tIOState;

typedef struct
{
	GPIO_TypeDef* GPIOx;
	uint16_t GPIO_Pin;
	uint8_t MODE;
	uint8_t SPEED;
	uint8_t TYPE;
	uint8_t PUPDR;
	tIOState DefState;
} tGPIO_Line;

void IO_Init(void);
void IO_SetLine(tIOLine Line, bool State);
bool IO_GetLine(tIOLine Line);
void IO_ConfigLine(tIOLine Line, uint8_t Mode, uint8_t Speed, uint8_t Type, uint8_t pupdr, tIOState State);

#endif
