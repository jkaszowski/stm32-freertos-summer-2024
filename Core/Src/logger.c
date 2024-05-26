#include "main.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "logger.h"
#include "string.h"
#include <stdio.h>

extern UART_HandleTypeDef huart2;

void print(char* fmt, ...){
	static char local_buffer[256];
	va_list args;
	va_start(args, fmt);
	vsprintf( local_buffer,fmt, args);
	va_end(args);
	HAL_UART_Transmit_DMA(&huart2, (uint8_t*)local_buffer, strlen(local_buffer));
}

void println(char* fmt, ...){
	static char local_buffer[256];
	va_list args;
	va_start(args, fmt);
	vsprintf( local_buffer, fmt, args);
	va_end(args);
	int end = strlen(local_buffer);
	if(end < 510){
		local_buffer[end] = '\r';
		local_buffer[end+1] = '\n';
		local_buffer[end+2] = '\0';
	}
	HAL_UART_Transmit_DMA(&huart2, (uint8_t*)local_buffer, strlen(local_buffer));
}
