#include "tasks.h"
#include "logger.h"
#include "parser.h"
#include "queue.h"
#include "bmp280.h"
#include "task.h"

osThreadId_t blinkTaskHandle;
const osThreadAttr_t blinkTask_attributes = {
  .name = "blinkTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

osThreadId_t buttonTaskHandle;
const osThreadAttr_t buttonTask_attributes = {
  .name = "blinkTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

osThreadId_t uartReceiverTaskHandle;
const osThreadAttr_t uartReceiverTask_attributes = {
  .name = "uartReceiverTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};

osThreadId_t uartSenderTaskHandle;
const osThreadAttr_t uartSenderTask_attributes = {
  .name = "uartSenderTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};

osThreadId_t bmpTaskHandle;
const osThreadAttr_t bmpTask_attributes = {
  .name = "bmpTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};

QueueHandle_t delayQueueHandle;
StaticQueue_t delayQueue;

QueueHandle_t messageQueueHandle;
StaticQueue_t messageQueue;

uint32_t frequency = 1;

#define DELAY_QUEUE_LEN 10
#define DELAY_QUEUE_ITEM_SIZE sizeof(int)

#define MESSAGE_QUEUE_LEN 10
#define MESSAGE_QUEUE_ITEM_SIZE 256

void initializeTasks()
{
	static uint8_t delay_queue_buffer[DELAY_QUEUE_LEN * DELAY_QUEUE_ITEM_SIZE];
	delayQueueHandle = xQueueCreateStatic(DELAY_QUEUE_LEN, DELAY_QUEUE_ITEM_SIZE, delay_queue_buffer, &delayQueue);

	static uint8_t message_queue_buffer[MESSAGE_QUEUE_LEN * MESSAGE_QUEUE_ITEM_SIZE];
	messageQueueHandle = xQueueCreateStatic(MESSAGE_QUEUE_LEN, MESSAGE_QUEUE_ITEM_SIZE, message_queue_buffer, &messageQueue);
}

void ButtonTask(void *argument)
{
	while(1)
	{
		if(HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == GPIO_PIN_RESET)
		{
			uint32_t delay = 1000/(++frequency);
			println("Button pressed, f=%d, delay=%d", frequency, delay);
			xQueueSend(delayQueueHandle, &delay, 0);
			while(HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == GPIO_PIN_RESET)
			{
				osDelay(100);
			}
		}
		osDelay(20);
	}
}

void BlinkTask(void *argument)
{
	static uint32_t delay = 1000;
	while(1)
	{
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		uint32_t tmp;
		BaseType_t ret = xQueueReceive(delayQueueHandle, &tmp, 0);
		if(ret == pdTRUE)
			delay = tmp;
		osDelay(delay);
	}
}

extern char uart_buffer[512];

void UartReceiverTask(void* argument)
{
	while(1)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		ParsingResult result = parse(uart_buffer);
		if(result.action == FREQUENCY_READ)
		{
			println("Current f=%d", frequency);
		}
		else if(result.action == FREQUENCY_SET)
		{
			frequency = result.value;
			uint32_t delay = 1000/frequency;
			xQueueSend(delayQueueHandle, &delay,0);
		}
	}
}

extern UART_HandleTypeDef huart2;
void UartSenderTask(void* argument){
	while(1){
//		uint8_t tmp[256];
//		BaseType_t ret = xQueueReceive(messageQueueHandle, &tmp, 0);
//		if(ret == pdTRUE)
//		{
//			HAL_UART_Transmit_DMA(&huart2, (uint8_t*)tmp, strlen(tmp));
//		}
//		println("ok");
		osDelay(100);
	}

}

extern SPI_HandleTypeDef hspi3;

void readBMP(){
	uint8_t toBeSent[7] = {0xF7, 0, 0, 0, 0, 0, 0};
	uint8_t answer[7] = {};
	HAL_GPIO_WritePin(BMP_CS_GPIO_Port, BMP_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi3, toBeSent, answer, 7, 100);
	HAL_GPIO_WritePin(BMP_CS_GPIO_Port, BMP_CS_Pin, GPIO_PIN_SET);
	int32_t pressureRaw = 0;
	int32_t temperatureRaw = 0;
	pressureRaw = answer[1] << 12 | answer[2] << 4 | answer[3] >> 4;
	temperatureRaw = answer[4] << 12 | answer[5] << 4 | answer[6] >> 4;
	double temperature = bmp280_compensate_T_double(temperatureRaw);
	double pressure = bmp280_compensate_P_double(pressureRaw);
	println("Temperature: %f\r\nPressure: %f", temperature, pressure);
//		println("Raw: %x %x %x %x %x %x", answer[1], answer[2], answer[3],answer[4],answer[5],answer[6]);
}

void BmpTask(void* argument){
	uint8_t answer[7] = {};
	uint8_t config[] = {0xf4 & 0b01111111, 0b01101111, 0xf5& 0b01111111, 0b00010000, 0xe0& 0b01111111, 0};
	HAL_GPIO_WritePin(BMP_CS_GPIO_Port, BMP_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi3, config, answer, 6, 100);
	HAL_GPIO_WritePin(BMP_CS_GPIO_Port, BMP_CS_Pin, GPIO_PIN_SET);
	while(1){
		vTaskSuspendAll();
		readBMP();
		xTaskResumeAll();
		osDelay(1000);
	}
}
