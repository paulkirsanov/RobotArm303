#ifndef __stepmotor_h
#define __stepmotor_h

#define configDEBUG	0

#include "stm32f30x.h"
#include "main.h"
#include "usart.h"

#define STEPPER_COUNT (3)

#define CORRDIR1 					0x54
#define CORRDIR2 					0x44
#define CORRDIR3 					0x64
#define CORRDIR4 					0x64

#define WRONGDIR1					0x70
#define WRONGDIR2					0x71
#define WRONGDIR3					0x72
#define WRONGDIR4					0x73

#define RESETDIR1					0x58
#define RESETDIR2					0x59
#define RESETDIR3					0x60

#define MOTOR1_MAX				8000
#define MOTOR2_MAX				4000
#define MOTOR3_MAX				8000

#define STARTPOSMOTOR1		5000
#define STARTPOSMOTOR2		2000
#define STARTPOSMOTOR3		100

typedef struct
{
	tIOLine DIR;
	tIOLine STEP;
	tIOLine EN;
	tIOLine MS0;
	tIOLine MS1;
	tIOLine MS2;
	tIOLine SNS;
} tStepper;

typedef enum
{
	st_Motor1 = 0,
	st_Motor2 = 1,
	st_Motor3 = 2
} tMotor;

typedef enum
{
	CCW = 0,
	CW = 1,
	NONE = 2
} tMotorDir;

typedef enum
{
	FULL_STEP = 0,
	HALF_STEP,
	QUARTER_STEP,
	EIGHTH_STEP,
	SIXTEENTH_STEP
} tMotorMode;

typedef struct
{
  tMotor aMotor;
  tMotorDir aDirection;
  uint16_t aSteps;
  uint8_t aSpeed;
	tMotorMode aMode;
} tRotate;

enum eStatus { BOOT, WORK };

void Stepper_Init(void);
bool sensor_read(tMotor Number);
uint8_t st_Rotate(tRotate tMotor, SemaphoreHandle_t Stopper, enum eStatus flag);
void servo_Rotate(uint8_t value, enum eStatus flag);

extern QueueHandle_t qMotor1, qMotor2, qMotor3, qGripper;
extern QueueHandle_t qValue1, qValue2, qValue3, qValue4;

extern SemaphoreHandle_t StopMotor1, StopMotor2, StopMotor3;
extern SemaphoreHandle_t OK_Motor1, OK_Motor2, OK_Motor3, OK_Gripper;

#endif
