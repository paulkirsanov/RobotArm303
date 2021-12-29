#include "stepmotor.h"

TaskHandle_t hMotor1, hMotor2, hMotor3, hGripper;

tStepper Motor[STEPPER_COUNT] ={{ io_DIR1, io_STEP1, io_EN1, io_MS10, io_MS11, io_MS12, io_SNS1 },
                                { io_DIR2, io_STEP2, io_EN2, io_MS20, io_MS21, io_MS22, io_SNS2 },
																{ io_DIR3, io_STEP3, io_EN3, io_SNS3 }
};

void st_Step(tMotor Number, tMotorDir Direction)
{
	IO_SetLine(Motor[Number].DIR, Direction);
	IO_SetLine(Motor[Number].STEP, LOW);
	taskYIELD();
	IO_SetLine(Motor[Number].STEP, HIGH);
}

void set_resolution(tMotor Number, tMotorMode resolution)
{
	switch(resolution)
	{
		case FULL_STEP:
			IO_SetLine(Motor[Number].MS0, LOW);
			IO_SetLine(Motor[Number].MS1, LOW);
			IO_SetLine(Motor[Number].MS2, LOW);
			break;
		case HALF_STEP:
			IO_SetLine(Motor[Number].MS0, HIGH);
			IO_SetLine(Motor[Number].MS1, LOW);
			IO_SetLine(Motor[Number].MS2, LOW);
			break;
		case QUARTER_STEP:
			IO_SetLine(Motor[Number].MS0, LOW);
			IO_SetLine(Motor[Number].MS1, HIGH);
			IO_SetLine(Motor[Number].MS2, LOW);
			break;
		case EIGHTH_STEP:
			IO_SetLine(Motor[Number].MS0, HIGH);
			IO_SetLine(Motor[Number].MS1, HIGH);
			IO_SetLine(Motor[Number].MS2, LOW);
			break;
		case SIXTEENTH_STEP:
			IO_SetLine(Motor[Number].MS0, HIGH);
			IO_SetLine(Motor[Number].MS1, HIGH);
			IO_SetLine(Motor[Number].MS2, HIGH);
			break;
		default:
			break;
	}
}

void SendQueueValue(QueueHandle_t qValue, uint16_t value, uint8_t data)
{
	xQueueSendToBack(qValue, &value, portMAX_DELAY);
	usart_send_data(USART1, data);
}

uint8_t RotateMotor(tMotor Number, tMotorDir Direction, uint16_t N, uint16_t Speed, uint16_t tmp, bool flag, int x, uint16_t i)
{
	if(flag == 1)
	{
		tmp = 0;
		SendQueueValue(qValue1, tmp, CORRDIR1);												//correct direction motor
		return 1;
	}
	else if(flag == 0)
	{
		if((x + tmp) < 0)
		{
			SendQueueValue(qValue1, tmp, WRONGDIR1);										//wrong direction motor
			return 1;
		}
		else
		{
			tmp = x + tmp;
			if(i == (N - 1))
			{
				SendQueueValue(qValue1, tmp, CORRDIR1);										//correct direction motor
			}
			st_Step(Number,Direction);
			vTaskDelay(Speed);
			return 2;
		}
	}
}

void st_Rotate(tMotor Number, tMotorDir Direction, uint16_t N, uint16_t Speed, SemaphoreHandle_t Stopper, tMotorMode Mode, bool flag)
{
	uint16_t i;
	uint16_t tmp1 = 0;
	uint16_t tmp2 = 0;
	uint16_t tmp3 = 0;
	int x = 1;
	
	IO_SetLine(Motor[Number].EN, OFF);
	
	if(Number != st_Motor3)
		set_resolution(Number, Mode);
	
	if(flag == 0)
	{
		if(Number == st_Motor1)
			xQueueReceive(qValue1, &tmp1, portMAX_DELAY);
		else if(Number == st_Motor2)
			xQueueReceive(qValue2, &tmp2, portMAX_DELAY);
		else if(Number == st_Motor3)
			xQueueReceive(qValue3, &tmp3, portMAX_DELAY);
	}
	
	if(Direction == CCW)
		x = -x;
	
	for(i = 0; i < N; i++)
	{
		if(xSemaphoreTake(Stopper, 0))
		{
			if(Stopper == StopMotor1)
			{
				if(flag == 1)
				{
					tmp1 = 0;
					SendQueueValue(qValue1, tmp1, CORRDIR1);												//correct direction motor 1
					break;
				}
				else if(flag == 0)
				{
					if((x + tmp1) < 0)
					{
						SendQueueValue(qValue1, tmp1, WRONGDIR1);										//wrong direction motor 1
						break;
					}
					else
					{
						tmp1 = x + tmp1;
						if(i == (N - 1))
						{
							SendQueueValue(qValue1, tmp1, CORRDIR1);										//correct direction motor 1
						}
						st_Step(Number, Direction);
						vTaskDelay(Speed);
						continue;
					}
				}
			}
			else if(Stopper == StopMotor2)
			{
				if(flag == 1)
				{
					tmp2 = 0;
					SendQueueValue(qValue2, tmp2, CORRDIR2);												//correct direction motor 2
					break;
				}
				else if(flag == 0)
				{
					if((x + tmp2) < 0)
					{
						tmp2 = 0;
						SendQueueValue(qValue2, tmp2, WRONGDIR2);										//wrong direction motor 2
						break;
					}
					else
					{
						tmp2 = x + tmp2;
						if(i == (N - 1))
						{
							SendQueueValue(qValue2, tmp2, CORRDIR2);										//correct direction motor 2
						}
						st_Step(Number, Direction);
						vTaskDelay(Speed);
						continue;
					}
				}
			}
			else if(Stopper == StopMotor3)
			{
				if(flag == 1)
				{
					tmp3 = 0;
					SendQueueValue(qValue3, tmp3, CORRDIR3);												//correct direction motor 3
					break;
				}
				else if(flag == 0)
				{
					if((x + tmp3) < 0)
					{
						SendQueueValue(qValue3, tmp3, WRONGDIR3);										//wrong direction motor 3
						break;
					}
					else
					{
						tmp3 = x + tmp3;
						if(i == (N - 1))
						{
							SendQueueValue(qValue3, tmp3, CORRDIR3);										//correct direction motor 3
						}
						st_Step(Number, Direction);
						vTaskDelay(Speed);
						continue;
					}
				}
			}
		}
		else
		{
			if(Number == st_Motor1)
			{
				if(((x + tmp1) < 0) || ((x + tmp1) > MOTOR1_MAX))
				{
					SendQueueValue(qValue1, tmp1, WRONGDIR1);											//wrong direction motor 1
					break;
				}
				else
				{
					tmp1 = x + tmp1;
					if(i == (N - 1))
					{	
						SendQueueValue(qValue1, tmp1, CORRDIR1);											//correct direction motor 1
					}
					st_Step(Number, Direction);
					vTaskDelay(Speed);
					continue;
				}
			}
			else if(Number == st_Motor2)
			{
				if(((x + tmp2) < 0) || ((x + tmp2) > MOTOR2_MAX))
				{
					tmp2 = 0;
					SendQueueValue(qValue2, tmp2, WRONGDIR2);											//wrong direction motor 2
					break;
				}
				else
				{
					tmp2 = x + tmp2;
					if(i == (N - 1))
					{
						SendQueueValue(qValue2, tmp2, CORRDIR2);											//correct direction motor 2
					}
					st_Step(Number, Direction);
					vTaskDelay(Speed);
					continue;
				}
			}
			else if(Number == st_Motor3)
			{
				if(((x + tmp3) < 0) || ((x + tmp3) > MOTOR3_MAX))
				{
					SendQueueValue(qValue3, tmp3, WRONGDIR3);											//wrong direction motor 3
					break;
				}
				else
				{
					tmp3 = x + tmp3;
					if(i == (N - 1))
					{
						SendQueueValue(qValue3, tmp3, CORRDIR3);											//correct direction motor 3
					}
					st_Step(Number, Direction);
					vTaskDelay(Speed);
					continue;
				}
			}
		}
	}
}

void servo_Rotate(uint8_t value)
{
	/*if(value == 0x55)				//open
	{
		TIM4->CCR3 = 7500 - (10 * 35);
		TIM4->CCR4 = 7500 - (85 * 35);
	}	
	else if(value == 0x14)		//close
	{
		TIM4->CCR3 = 7500 - (85 * 35);
		TIM4->CCR4 = 7500 - (10 * 35);
	}*/
	
	TIM4->CCR4 = 7500 - (value * 35);							//servo 1
	TIM4->CCR3 = 7500 - ((90 - value) * 35);			//servo 2
}

bool sensor_read(tMotor Number)
{	
	if(IO_GetLine(Motor[Number].SNS) == true)
		return true;
	else
		return false;
}

void st_vMotor1(void *pvParameters)
{
	tRotate Rcv;
	uint16_t tmp = 0;
	uint8_t tmp_l = 0, tmp_h = 0;
	
	while(1)
	{
		if(xQueueReceive(qMotor1, &Rcv, portMAX_DELAY))
		{
			st_Rotate(Rcv.aMotor, Rcv.aDirection, Rcv.aSteps, Rcv.aSpeed, StopMotor1, Rcv.aMode, 0);
			
			xQueuePeek(qValue1, &tmp, portMAX_DELAY);
			tmp_h = tmp >> 8;
			tmp_l = tmp & 0xFF;
			usart_send_data(USART1, 0x23);
			usart_send_data(USART1, tmp_l);
			usart_send_data(USART1, tmp_h);
			
			xSemaphoreGive(OK_Motor1);
		}
	}
}

void st_vMotor2(void *pvParameters)
{
	tRotate Rcv;
	uint16_t tmp = 0;
	uint8_t tmp_l = 0, tmp_h = 0;

	while(1)
	{
		if(xQueueReceive(qMotor2, &Rcv, portMAX_DELAY))
		{
			st_Rotate(Rcv.aMotor, Rcv.aDirection, Rcv.aSteps, Rcv.aSpeed, StopMotor2, Rcv.aMode, 0);
			
			xQueuePeek(qValue2, &tmp, portMAX_DELAY);
			tmp_h = tmp >> 8;
			tmp_l = tmp & 0xFF;
			usart_send_data(USART1, 0x24);
			usart_send_data(USART1, tmp_l);
			usart_send_data(USART1, tmp_h);
			
			xSemaphoreGive(OK_Motor2);
		}
	}
}

void st_vMotor3(void *pvParameters)
{
	tRotate Rcv;
	uint16_t tmp = 0;
	uint8_t tmp_l = 0, tmp_h = 0;

	while(1)
	{
		if(xQueueReceive(qMotor3, &Rcv, portMAX_DELAY))
		{
			st_Rotate(Rcv.aMotor, Rcv.aDirection, Rcv.aSteps, Rcv.aSpeed, StopMotor3, Rcv.aMode, 0);
			
			xQueuePeek(qValue3, &tmp, portMAX_DELAY);
			tmp_h = tmp >> 8;
			tmp_l = tmp & 0xFF;
			usart_send_data(USART1, 0x25);
			usart_send_data(USART1, tmp_l);
			usart_send_data(USART1, tmp_h);
			
			xSemaphoreGive(OK_Motor3);
		}
	}
}

void st_vGripper(void *pvParameters)
{
	uint8_t Rcv_G;
	//xSemaphoreTake(gMutex, portMAX_DELAY);
	
	while(1)
	{
		if(xQueueReceive(qGripper, &Rcv_G, portMAX_DELAY))
		{
			servo_Rotate(Rcv_G);
			xSemaphoreGive(OK_Gripper);
		}
	}
	//vTaskDelay(1000);
	//xSemaphoreGive(gMutex);
}

void Stepper_Init(void)
{
	/* ************** MOTOR 1 START ************** */
	
	if(pdTRUE != xTaskCreate(st_vMotor1, "Motor1", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, hMotor1))
		ERROR_ACTION(TASK_NOT_CREATE, 0);
	
	qMotor1 = xQueueCreate(50, sizeof(tRotate));
	if( qMotor1 != NULL )
	{
		#if(configDEBUG)
			usart_send_string(USART1, "Queue qMotor1 was create\r\n");
		#endif
	}
	
	qValue1 = xQueueCreate(1, sizeof(uint16_t));
	if( qValue1 != NULL )
	{
		#if(configDEBUG)
			usart_send_string(USART1, "Queue qValue1 was create\r\n");
		#endif
	}
	
	StopMotor1 = xSemaphoreCreateBinary();
	if( StopMotor1 != NULL )
	{
		#if(configDEBUG)
			usart_send_string(USART1, "StopMotor1 was create\r\n");
		#endif
	}
	
	OK_Motor1 = xSemaphoreCreateBinary();
	if( OK_Motor1 != NULL )
	{
		#if(configDEBUG)
			usart_send_string(USART1, "OK_Motor1 was create\r\n");
		#endif
	}
	
	/* ************** MOTOR 1 END ************** */
	
	/* ************** MOTOR 2 START ************** */

	if(pdTRUE != xTaskCreate(st_vMotor2, "Motor2", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, hMotor2))
		ERROR_ACTION(TASK_NOT_CREATE, 0);
	
	qMotor2 = xQueueCreate(50, sizeof(tRotate));
	if( qMotor2 != NULL )
	{
		#if(configDEBUG)
			usart_send_string(USART1, "Queue qMotor2 was create\r\n");
		#endif
	}
	
	qValue2 = xQueueCreate(1, sizeof(uint16_t));
	if( qValue2 != NULL )
	{
		#if(configDEBUG)
			usart_send_string(USART1, "Queue qValue2 was create\r\n");
		#endif
	}
	
	StopMotor2 = xSemaphoreCreateBinary();
	if( StopMotor2 != NULL )
	{
		#if(configDEBUG)
			usart_send_string(USART1, "StopMotor2 was create\r\n");
		#endif
	}
	
	OK_Motor2 = xSemaphoreCreateBinary();
	if( OK_Motor2 != NULL )
	{
		#if(configDEBUG)
			usart_send_string(USART1, "OK_Motor2 was create\r\n");
		#endif
	}
	
	/* ************** MOTOR 2 END ************** */
	
	/* ************** MOTOR 3 START ************** */
	
	if(pdTRUE != xTaskCreate(st_vMotor3, "Motor3", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, hMotor3))
		ERROR_ACTION(TASK_NOT_CREATE, 0);
	
	qMotor3 = xQueueCreate(50, sizeof(tRotate));
	if( qMotor3 != NULL )
	{
		#if(configDEBUG)
			usart_send_string(USART1, "Queue qMotor3 was create\r\n");
		#endif
	}
	
	qValue3 = xQueueCreate(1, sizeof(uint16_t));
	if( qValue3 != NULL )
	{
		#if(configDEBUG)
			usart_send_string(USART1, "Queue qValue3 was create\r\n");
		#endif
	}
	
	StopMotor3 = xSemaphoreCreateBinary();
	if( StopMotor3 != NULL )
	{
		#if(configDEBUG)
			usart_send_string(USART1, "StopMotor3 was create\r\n");
		#endif
	}
	
	OK_Motor3 = xSemaphoreCreateBinary();
	if( OK_Motor3 != NULL )
	{
		#if(configDEBUG)
			usart_send_string(USART1, "OK_Motor3 was create\r\n");
		#endif
	}
	
	/* ************** MOTOR 3 END ************** */
	/* ************* GRIPPER START ************* */
	
	if(pdTRUE != xTaskCreate(st_vGripper, "Gripper", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, hGripper))
		ERROR_ACTION(TASK_NOT_CREATE, 0);
	
	qGripper = xQueueCreate(5, sizeof(uint8_t));
	if( qGripper != NULL )
	{
		#if(configDEBUG)
			usart_send_string(USART1, "Queue qGripper was create\r\n");
		#endif
	}
	
	OK_Gripper = xSemaphoreCreateBinary();
	if( OK_Gripper != NULL )
	{
		#if(configDEBUG)
			usart_send_string(USART1, "OK_Gripper was create\r\n");
		#endif
	}
	
	/*gMutex = xSemaphoreCreateMutex();
	if( gMutex != NULL )
	{
		#if(configDEBUG)
			usart_send_string(USART1, "OK_Gripper was create\r\n");
		#endif
	}*/
	
	/* ************** GRIPPER END ************** */
}
