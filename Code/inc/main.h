
#ifndef __main_h
#define __main_h

#include "stm32f30x.h"

#include <stdbool.h>
#include "inttypes.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "io.h"
#include "stepmotor.h"

#include "timer.h"
#include "led.h"
#include "usart.h"
#include "utils.h"
#include "crc.h"

#define	ERROR_ACTION(CODE,POS)		do{}while(0)

#define 	RX_BUFFER_SIZE	16
#define 	RX_PACKET_SIZE	12
#define 	RX_CRC_SIZE			4
	
#define configDEBUG_BOOTLOADER																0
#define configDEBUG_SETSTARTPOSITION													1

#endif
