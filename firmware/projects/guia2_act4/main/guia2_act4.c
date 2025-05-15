/*! @mainpage Actividad 4 de la guia 2
 *
 * @section se resuelve la actividad 4
 *
 * This section describes how the program works.
 *
 * @author Maria Victoria Boretto
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "timer_mcu.h"
#include "uart_mcu.h"
#include "analog_io_mcu.h"
/*==================[macros and definitions]=================================*/
#define SAMPLE_PERIOD_US 2000 // 2 ms
#define TAMAÑO_DE_BUFFER 231

/*==================[internal data definition]===============================*/
TaskHandle_t adc_task_handle = NULL;
TaskHandle_t dac_task_handle = NULL;
TaskHandle_t manejador_de_tareas_principal = NULL;
const char ecg[TAMAÑO_DE_BUFFER] = {
    76, 77, 78, 77, 79, 86, 81, 76, 84, 93, 85, 80,
    89, 95, 89, 85, 93, 98, 94, 88, 98, 105, 96, 91,
    99, 105, 101, 96, 102, 106, 101, 96, 100, 107, 101,
    94, 100, 104, 100, 91, 99, 103, 98, 91, 96, 105, 95,
    88, 95, 100, 94, 85, 93, 99, 92, 84, 91, 96, 87, 80,
    83, 92, 86, 78, 84, 89, 79, 73, 81, 83, 78, 70, 80, 82,
    79, 69, 80, 82, 81, 70, 75, 81, 77, 74, 79, 83, 82, 72,
    80, 87, 79, 76, 85, 95, 87, 81, 88, 93, 88, 84, 87, 94,
    86, 82, 85, 94, 85, 82, 85, 95, 86, 83, 92, 99, 91, 88,
    94, 98, 95, 90, 97, 105, 104, 94, 98, 114, 117, 124, 144,
    180, 210, 236, 253, 227, 171, 99, 49, 34, 29, 43, 69, 89,
    89, 90, 98, 107, 104, 98, 104, 110, 102, 98, 103, 111, 101,
    94, 103, 108, 102, 95, 97, 106, 100, 92, 101, 103, 100, 94, 98,
    103, 96, 90, 98, 103, 97, 90, 99, 104, 95, 90, 99, 104, 100, 93,
    100, 106, 101, 93, 101, 105, 103, 96, 105, 112, 105, 99, 103, 108,
    99, 96, 102, 106, 99, 90, 92, 100, 87, 80, 82, 88, 77, 69, 75, 79,
    74, 67, 71, 78, 72, 67, 73, 81, 77, 71, 75, 84, 79, 77, 77, 76, 76,
};
/*==================[internal functions declaration]=========================*/
/**
 * @brief Función invocada en la interrupción del timer A
 */
void FuncTimerA(void* param){
    vTaskNotifyGiveFromISR(adc_task_handle, pdFALSE);    /* Envía una notificación a la tarea asociada a adc */
}
/**
 * @brief Función invocada en la interrupción del timer B
 */
void FuncTimerB(void* param){
    vTaskNotifyGiveFromISR(dac_task_handle, pdFALSE);    /* Envía una notificación a la tarea asociada a dac */
}
/**
 * @brief Tarea que se encarga de la conversion AD, recibe el valor en analogo y lo convierte en formato ASCII para enviarlo por UART
 */
static void ADCTask(void *pvParameter){
	uint16_t adc_value = 0;

	while(true){
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		AnalogInputReadSingle(CH1, &adc_value);

		// Convertir valor a formato ASCII y enviarlo por UART
		UartSendString(UART_PC, ">CH1:");
        UartSendString(UART_PC, (char*)UartItoa(adc_value, 10));
		UartSendString(UART_PC, " \r\n");
	}
}

/**
 * @brief Tarea que se encarga de la conversion DA utilizando los datos pasados por la lista ecg definida anteriormente
 */
static void DACTask(void *pvParameter){
	uint16_t i = 0;
	while(true){
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		AnalogOutputWrite(ecg[i]);
		i++;
		if (i >= TAMAÑO_DE_BUFFER) {
        	i = 0; // Reinicia la señal para que sea continua
    }
	}
	

}

/*==================[external functions definition]==========================*/
void app_main(void){
	timer_config_t timer_config_a = {
    .timer = TIMER_A,
    .period = SAMPLE_PERIOD_US, 
    .func_p = FuncTimerA,
    .param_p = NULL
	};
	TimerInit(&timer_config_a);

	timer_config_t timer_config_b = {
    .timer = TIMER_B,
    .period = SAMPLE_PERIOD_US/2, 
    .func_p = FuncTimerB,
    .param_p = NULL
	};
	TimerInit(&timer_config_b);

	serial_config_t my_uart = {
		.port = UART_PC,
		.baud_rate = 115200,  // suficiente para enviar datos a 500Hz
		.func_p = NULL,
		.param_p = NULL
	};
	UartInit(&my_uart);

	analog_input_config_t adc_config = {
		.input = CH1,
		.mode = ADC_SINGLE, // o como esté definido en tu plataforma
		.func_p = NULL,
		.param_p = NULL
	};
	AnalogInputInit(&adc_config);
	AnalogOutputInit();

	xTaskCreate(&ADCTask, "ADC_UART", 512, NULL, 5, &adc_task_handle);
	xTaskCreate(&DACTask, "DAC_UART", 512, NULL, 5, &dac_task_handle);

	TimerStart(TIMER_A);
	TimerStart(TIMER_B);
}
/*==================[end of file]============================================*/