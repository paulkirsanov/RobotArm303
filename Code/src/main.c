#include "main.h"

QueueHandle_t qMotor1, qMotor2, qMotor3, qGripper;
QueueHandle_t qValue1, qValue2, qValue3;

SemaphoreHandle_t StopMotor1 = NULL, StopMotor2 = NULL, StopMotor3 = NULL;
SemaphoreHandle_t OK_Motor1 = NULL, OK_Motor2 = NULL, OK_Motor3 = NULL, OK_Gripper = NULL;
SemaphoreHandle_t ISRFromUSARTHandle;

TaskHandle_t hBootLoader, hBlinker, hSender, hBlinkerBootLoader, hReadSemaphore;

uint8_t rx_index = 0;

uint8_t packet_rx[RX_PACKET_SIZE] = {0};
uint8_t crc_rx[RX_CRC_SIZE] = {0};
uint32_t crc_receive = 0;
uint32_t crc_calculation = 0;
uint8_t index_crc = 0;
uint16_t error_flag = 0;
uint16_t count_packet = 0;

volatile static float CPU_USAGE = 0;

static tMotor sensor = st_Motor1;

void rcc_init(void);
void vBlinker(void *pvParameters);
void vSender(void *pvParameters);
void vBootLoader(void *pvParameters);
void vBlinkerBootLoader(void *pvParameters);
void vReadSemaphore(void *pvParameters);

int main(void)
{
	rcc_init();
	led_init();
	usart1_init();
	crc32_init();
	
	IO_Init();
	Stepper_Init();
	
	timer2_init();
//	timer3_init();
	timer4_pwm_init();
	
	if(pdTRUE != xTaskCreate(vReadSemaphore, "ReadSemaphore", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &hReadSemaphore))
		ERROR_ACTION(TASK_NOT_CREATE, 0);
	
	if(pdTRUE != xTaskCreate(vBootLoader, "BootLoader", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &hBootLoader))
		ERROR_ACTION(TASK_NOT_CREATE, 0);
	
	if(pdTRUE != xTaskCreate(vBlinkerBootLoader, "BlinkerBootLoader", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &hBlinkerBootLoader))
		ERROR_ACTION(TASK_NOT_CREATE, 0);
	
	if(pdTRUE != xTaskCreate(vBlinker, "Blinker", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &hBlinker))
		ERROR_ACTION(TASK_NOT_CREATE, 0);
	
	if(pdTRUE != xTaskCreate(vSender, "Sender", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &hSender))
		ERROR_ACTION(TASK_NOT_CREATE, 0);
	
	vTaskStartScheduler();
	
	return 0;
}

void rcc_init(void)
{
	RCC->CR |= ((uint32_t)RCC_CR_HSEON);												//Enable HSE
	while(!(RCC->CR & RCC_CR_HSERDY));													//Ready start HSE
	
	FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY;					//Clock flash memory
	
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;														//AHB = SYSCLK/1
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV1;														//APB1 = HCLK/1
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV4;														//APB2 = HCLK/4
	
	RCC->CFGR &= ~RCC_CFGR_PLLMULL;															//clear PLLMULL bits
	RCC->CFGR &= ~RCC_CFGR_PLLSRC;															//clear PLLSRC bits
	RCC->CFGR &= ~RCC_CFGR_PLLXTPRE;														//clear PLLXPRE bits
	
	RCC->CFGR |= RCC_CFGR_PLLSRC;																//source HSE
	RCC->CFGR |= RCC_CFGR_PLLXTPRE;															//sorce HSE/1 = 8MHz
	RCC->CFGR |= RCC_CFGR_PLLMULL9;															//PLL x9: clock = 8MHz * 9 = 72 MHz
	
	RCC->CR |= RCC_CR_PLLON;																		//enable PLL
	while(!(RCC->CR & RCC_CR_PLLRDY));													//wait till PLL is ready
	
	RCC->CFGR &= ~RCC_CFGR_SW;																	//clear SW bits
	RCC->CFGR |= RCC_CFGR_SW_PLL;																//select surce SYSCLK = PLL
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_1){}				//wait till PLL is used
}

void vBlinker(void *pvParameters)
{
	while(1)
	{
		GPIOE->BSRR |= GPIO_BSRR_BS_11;
		vTaskDelay(500);
		GPIOE->BSRR |= GPIO_BSRR_BR_11;
		vTaskDelay(500);
	}
}

void vBlinkerBootLoader(void *pvParameters)
{
	while(1)
	{
		GPIOE->BSRR |= GPIO_BSRR_BS_13;
		vTaskDelay(100);
		GPIOE->BSRR |= GPIO_BSRR_BR_13;
		vTaskDelay(100);
	}
}

void vSender(void *pvParameters)
{
	tRotate Sendit;
	uint8_t servo_value;
	uint8_t x = 0;
	portBASE_TYPE xStatus1 = 0, xStatus2 = 0, xStatus3 = 0, xStatus4 = 0;
	
	ISRFromUSARTHandle = xSemaphoreCreateBinary();
	
	while(1)
	{
		if(xSemaphoreTake(ISRFromUSARTHandle, portMAX_DELAY))
		{
			if((int)packet_rx[0] == 0xF8)
			{
				switch(packet_rx[1])
				{
					case 0x01:
						Sendit.aMotor = st_Motor1;
						break;
					case 0x02:
						Sendit.aMotor = st_Motor2;
						break;
					case 0x03:
						Sendit.aMotor = st_Motor3;
						break;
					default:
						break;
				}
				
				switch(packet_rx[2])
				{
					case 0x52:
						Sendit.aDirection = CCW;
						break;
					case 0x55:
						Sendit.aDirection = CW;
						break;
					default:
						break;
				}
				
				switch(packet_rx[3])
				{
					case 0x46:
						Sendit.aMode = FULL_STEP;
						break;
					case 0x48:
						Sendit.aMode = HALF_STEP;
						break;
					case 0x51:
						Sendit.aMode = QUARTER_STEP;
						break;
					case 0x45:
						Sendit.aMode = EIGHTH_STEP;
						break;
					case 0x53:
						Sendit.aMode = SIXTEENTH_STEP;
						break;
					default:
						break;
				}
				
				Sendit.aSteps = convertFrom8to16(packet_rx[4], packet_rx[5]);
				Sendit.aSpeed = convertFrom8to16(packet_rx[6], packet_rx[7]);
				
				if((int)packet_rx[8] == 0xE0)
				{
					if(Sendit.aMotor == st_Motor1)
					{
						xStatus1 = xQueueSendToBack(qMotor1, &Sendit, portMAX_DELAY);
						if(xStatus1 != pdPASS)
						{
							usart_send_string(USART1, "Could not send to the queue.\r\n");
						}
					} else if(Sendit.aMotor == st_Motor2)
					{
						xStatus2 = xQueueSendToBack(qMotor2, &Sendit, portMAX_DELAY);
						if(xStatus2 != pdPASS)
						{
							usart_send_string(USART1, "Could not send to the queue.\r\n");
						}
					} else if(Sendit.aMotor == st_Motor3)
					{
						xStatus3 = xQueueSendToBack(qMotor3, &Sendit, portMAX_DELAY);
						if(xStatus3 != pdPASS)
						{
							usart_send_string(USART1, "Could not send to the queue.\r\n");
						}
					}
				}
			}
			else if((int)packet_rx[0] == 0xF9)
			{
				servo_value = packet_rx[1];
				
				packet_rx[2] = 0;
				packet_rx[3] = 0;
				packet_rx[4] = 0;
				packet_rx[5] = 0;
				packet_rx[6] = 0;
				packet_rx[7] = 0;
				
				if((int)packet_rx[8] == 0xE0)
				{
					xStatus4 = xQueueSendToBack(qGripper, &servo_value, portMAX_DELAY);
					if(xStatus4 != pdPASS)
					{
						usart_send_string(USART1, "Could not send to the queue.\r\n");
					}
				}
			}
			else
			{
				for(x = 0; x < RX_BUFFER_SIZE; x++)
				{
					packet_rx[x] = 0;
				}
				rx_index = 0;
			}
		}
		else
		{
			for(x = 0; x < RX_BUFFER_SIZE; x++)
			{
				packet_rx[x] = 0;
			}
			rx_index = 0;
		}
	}
}

void vBootLoader(void *pvParameters)
{
	NVIC_EnableIRQ(TIM2_IRQn);
	//NVIC_DisableIRQ(TIM3_IRQn);
	vTaskSuspend(hBlinker);
	vTaskSuspend(hSender);
	vTaskSuspend(hReadSemaphore);
	
	#if(configDEBUG_SETSTARTPOSITION)
	
	st_Rotate(st_Motor1, CW, 10000, 1, StopMotor1, SIXTEENTH_STEP, 1);
	vTaskDelay(10);
	usart_send_data(USART1, RESETDIR1);							//Reset value motor 1
	usart_send_data(USART1, 0x00);
	usart_send_data(USART1, 0x00);
	
	st_Rotate(st_Motor2, CW, 8000, 1, StopMotor2, SIXTEENTH_STEP, 1);
	vTaskDelay(10);
	usart_send_data(USART1, RESETDIR2);							//Reset value motor 2
	usart_send_data(USART1, 0x00);
	usart_send_data(USART1, 0x00);
	
	st_Rotate(st_Motor3, CW, 100, 1, StopMotor3, SIXTEENTH_STEP, 1);
	vTaskDelay(10);
	usart_send_data(USART1, RESETDIR3);						  //Reset value motor 3
	usart_send_data(USART1, 0x00);
	usart_send_data(USART1, 0x00);
	
	vTaskDelay(10);
	servo_Rotate(0x14);															//Close
	usart_send_data(USART1, 0x18);									//Servo close
	
	#endif
	
//	NVIC_DisableIRQ(TIM2_IRQn);
//	NVIC_EnableIRQ(TIM3_IRQn);
	vTaskResume(hBlinker);
	vTaskResume(hSender);
	vTaskResume(hReadSemaphore);
	
	vTaskDelay(10);
	usart_send_data(USART1, 0x30);									//MCU Ready
	
	vTaskSuspend(hBlinkerBootLoader);
	GPIOE->BSRR |= GPIO_BSRR_BR_13;
	vTaskSuspend(hBootLoader);
}

void USART1_IRQHandler(void)
{
	static portBASE_TYPE xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;
	
	if(USART1->ISR & USART_ISR_RXNE)
	{
		USART1->ISR &= ~USART_ISR_RXNE;
		
		if(rx_index < RX_PACKET_SIZE)
		{
			packet_rx[rx_index] = USART1->RDR;
			rx_index++;
		}
		else if(rx_index < RX_BUFFER_SIZE)
		{
			crc_rx[rx_index - RX_PACKET_SIZE] = USART1->RDR;
			rx_index++;
		}
		else if(rx_index == RX_BUFFER_SIZE)
		{
			rx_index = 0;
			++count_packet;
			
			crc_receive = convertFrom8to32(crc_rx);
			crc_calculation = crc_calc((uint32_t *)packet_rx, 3);
			
			if(crc_calculation != crc_receive)
			{
				++error_flag;
				usart_send_data(USART1, 0x0A);				// ERROR
			}
			else
			{
				xSemaphoreGiveFromISR(ISRFromUSARTHandle, &xHigherPriorityTaskWoken);
				usart_send_data(USART1, 0x1D);				// OK
			}
			
			if(xHigherPriorityTaskWoken == pdTRUE)
			{
				portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
			}
		}
	}
	else
	{
		NVIC_DisableIRQ(USART1_IRQn);
	}
}

void TIM2_IRQHandler(void)
{
	static portBASE_TYPE xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;
	
	if( TIM2->SR & TIM_SR_UIF )
	{
		TIM2->SR &= ~TIM_SR_UIF;
		
		if( sensor_read(sensor) == false )
		{
			if( sensor == st_Motor1 )
				xSemaphoreGiveFromISR(StopMotor1, &xHigherPriorityTaskWoken);
			else if( sensor == st_Motor2 )
				xSemaphoreGiveFromISR(StopMotor2, &xHigherPriorityTaskWoken);
			else if( sensor == st_Motor3 )
				xSemaphoreGiveFromISR(StopMotor3, &xHigherPriorityTaskWoken);
			
			if( xHigherPriorityTaskWoken == pdTRUE )
				portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
		}
		
		if( sensor == st_Motor3 )
			sensor = st_Motor1;
		else
			sensor++;
	}
}

void TIM3_IRQHandler(void)
{
	if( TIM3->SR & TIM_SR_UIF )
	{
		TIM3->SR &= ~TIM_SR_UIF;
		
	}
}

void vApplicationIdleHook(void)
{
	static portTickType LastTick;
	static int count;
	static int max_count;
	
	count++;
	if(xTaskGetTickCount()- LastTick > 1000)
	{
		LastTick = xTaskGetTickCount();
		if(count > max_count) max_count = count;
		CPU_USAGE = 100 -(100 * ((float)count / (float)max_count));
		
		count = 0;
	}
}

void vReadSemaphore(void *pvParameters)
{
	while(1)
	{
		if(xSemaphoreTake(OK_Motor1, 0))
			usart_send_data(USART1, 0x27);
		else if(xSemaphoreTake(OK_Motor2, 0))
			usart_send_data(USART1, 0x28);
		else if(xSemaphoreTake(OK_Motor3, 0))
			usart_send_data(USART1, 0x31);
		else if(xSemaphoreTake(OK_Gripper, 0))
			usart_send_data(USART1, 0x29);
		
		vTaskDelay(10);
	}
}
