#include "crc.h"

void crc32_init(void)
{
	RCC->AHBENR |= RCC_AHBENR_CRCEN;
	CRC->INIT = 0xFFFFFFFF;
	CRC->CR |= CRC_CR_RESET;
	while(CRC->CR & CRC_CR_RESET);
}

uint32_t crc_calc(uint32_t pBuffer[], uint32_t BufferLength)
{
	uint32_t index;
	uint32_t temp = 0U;
	
	CRC->CR |= CRC_CR_RESET;

	for (index = 0U; index < BufferLength; index++)
  {
		CRC->DR = pBuffer[index];
  }
	temp = CRC->DR;
	
	return temp;
}
