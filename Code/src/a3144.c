#include "a3144.h"

tHall Sensor[SENSOR_COUNT] = { io_SNS1, io_SNS2 };

uint8_t a3144_read(hSensor Number)
{	
	if(IO_GetLine(Sensor[Number].PIN) == true)
		return 0;
	else
		return 1;
}
