/*
 *
 * FreeRTOS tasks are located here
 *
 */

#ifndef TASKS_H
#define TASKS_H

#include "main.h"
#include "cmsis_os.h"

extern osThreadId_t blinkTaskHandle;
extern const osThreadAttr_t blinkTask_attributes;

extern osThreadId_t buttonTaskHandle;
extern const osThreadAttr_t buttonTask_attributes;

extern osThreadId_t uartReceiverTaskHandle;
extern const osThreadAttr_t uartReceiverTask_attributes;

extern osThreadId_t uartSenderTaskHandle;
extern const osThreadAttr_t uartSenderTask_attributes;

extern osThreadId_t bmpTaskHandle;
extern const osThreadAttr_t bmpTask_attributes;

void initializeTasks();

void ButtonTask(void *argument);

void BlinkTask(void *argument);

void UartReceiverTask(void* argument);

void UartSenderTask(void* argument);

void BmpTask(void* argument);

#endif
