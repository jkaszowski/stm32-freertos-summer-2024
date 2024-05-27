#include "main.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "logger.h"
#include "string.h"
#include <stdio.h>

extern UART_HandleTypeDef huart2;
extern QueueHandle_t messageQueueHandle;

void print(char* fmt, ...){
	static char local_buffer[256];
	va_list args;
	va_start(args, fmt);
	vsprintf( local_buffer,fmt, args);
	va_end(args);
	int end = strlen(local_buffer);
	if(end < 255)
		local_buffer[end] = 0;
	else
		local_buffer[255] = 0;

	xQueueSend(messageQueueHandle, local_buffer, 0);

}

void println(char* fmt, ...){
	static char local_buffer[256];
	va_list args;
	va_start(args, fmt);
	vsprintf( local_buffer, fmt, args);
	va_end(args);
	int end = strlen(local_buffer);
	if(end < 254){
		local_buffer[end] = '\r';
		local_buffer[end+1] = '\n';
		local_buffer[end+2] = '\0';
	}

	xQueueSend(messageQueueHandle, local_buffer, 0);
}
