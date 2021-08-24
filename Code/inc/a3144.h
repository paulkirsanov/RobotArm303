#ifndef __a3144_h
#define __a3144_h

#include "stm32f30x.h"
#include "io.h"

#define SENSOR_COUNT (2)

typedef struct
{
	tIOLine PIN;
} tHall;

typedef enum
{
	h_Sensor1 = 0
} hSensor;

uint8_t a3144_read(hSensor Number);

#endif
