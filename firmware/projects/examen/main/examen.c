/*! @mainpage Examen 09/06
 *
 * @section se resuelve el examen de EProg
 *
 * El ejercicio consiste en medir con el sensor de temperatura y humedad sus valores, y con esos datos prender led rojo
 * si hay riesgo de nevada, prender led amarillo si hay radiacion elevada, o prender led verde si no hay ningun riesgo.
 * Los valores de radiacion, temperatura y humedad se van a mostrar por la PC, indicando ademas si hay algun riesgo presente.
 *
 * 
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	PIN_9	 	| 	GPIO_3		|
 * | 	PIN_7	 	|    CH2     	|  //ADC
 * 
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 09/06/2025 | Document creation		                         |
 *
 * @author Maria Victoria Boretto (vickybore00@gmail.com)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "dht11.h"
#include "gpio_mcu.h"
#include "timer_mcu.h"
#include "uart_mcu.h"
#include "led.h" 
#include "switch.h"
#include "analog_io_mcu.h"
/*==================[macros and definitions]=================================*/
#define CONFIG_BLINK_PERIOD_MEDICION 1000  //cada 1 segundo mide
#define CONFIG_BLINK_PERIOD_RADIACION 5000  //cada 1 segundo mide
#define GPIO_3 3

bool successfull_read = false;
bool encendido = false; 
bool riesgo_nevada = false;
bool radiacion_elevada = false;
uint16_t humedad;
uint16_t temperatura;
uint16_t radiacion;

/*==================[internal data definition]===============================*/
TaskHandle_t medicion_task_handle = NULL;
TaskHandle_t radiacion_task_handle = NULL;

/*==================[internal functions declaration]=========================*/
/**
 * @brief Función invocada en la interrupción del timer A
 */
void FuncTimerMedicion(void* param){
    vTaskNotifyGiveFromISR(medicion_task_handle, pdFALSE);    /* Envía una notificación a la tarea asociada a medicion humedad temperatura */
}
/**
 * @brief Función invocada en la interrupción del timer B
 */
void FuncTimerRadiacion(void* param){
    vTaskNotifyGiveFromISR(radiacion_task_handle, pdFALSE);    /* Envía una notificación a la tarea asociada a radiacion */
}

/**
 * @brief Función invocada en la interrupción de la tecla 1, prende el dispositivo
 */
void FuncTecla1(void){
	encendido = !encendido;
}

/**
 * @brief Función invocada en la interrupción de la tecla 2, apaga el dispositivo
 */
void FuncTecla2(void){
	encendido = !encendido;
}

/**
 * @brief Tarea que se encarga de medir la radiacion
 */
static void MedicionRadiacionTask(void *pvParameter){
	while(true){
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		AnalogInputInit();
		AnalogInputReadSingle(CH1, radiacion);  // radiacion en mV
		radiacion = (radiacion*100) / 3300;  // valor de la radiacion en mR/h
		if(encendido){
			UartSendString(UART_PC, "Radiacion: \r");
			UartSendString(UART_PC, (char*)UartItoa(radiacion, 10));
			UartSendString(UART_PC, "mR/h \r\n");
		if (radiacion > 40) { // si la radiacion sobrepasa los 40mR/h
			radiacion_elevada = true;
			LedOn(LED_2); // enciendo led amarillo
			LedOff(LED_1);
			UartSendString(UART_PC, "Radiacion: \r");
			UartSendString(UART_PC, (char*)UartItoa(radiacion, 10));
			UartSendString(UART_PC, "mR/h - RADIACION ELEVADA\r\n");
		}
		
		
	}
}
}

/**
 * @brief Tarea que se encarga de medir la temperatura y la humedad
 */
static void MedicionHumedadTemperaturaTask(void *pvParameter){
	while(true){
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		successfull_read = dht11Read(humedad, temperatura);
		radiacion = 0;
		if(encendido){
			if(successfull_read){
			LedsOffAll(); // comienzo con los leds apagados
			if (humedad > 85 && (0 > temperatura > 2)) { // se da el riesgo de nevada
				riesgo_nevada = true;
				LedOn(LED_3); // enciendo led rojo
				LedOff(LED_1);
				UartSendString(UART_PC, "Temperatura: \r");
				UartSendString(UART_PC, (char*)UartItoa(temperatura, 10));
				UartSendString(UART_PC, "°C - \r");
				UartSendString(UART_PC, "Humedad: \r");
				UartSendString(UART_PC, (char*)UartItoa(humedad, 10));
				UartSendString(UART_PC, "% - RIESGO DE NEVADA\r\n");
			} else if(!riesgo_nevada && !radiacion_elevada){
				LedOn(LED_1); // enciendo led verde
				LedOff(LED_2);
				LedOff(LED_3);

			}
		}
		else{
			LedsOffAll();
		}
		
	}
}
}
		


/*==================[external functions definition]==========================*/
void app_main(void){
	LedsInit();
	dht11Init(GPIO_3);
	SwitchesInit();
	SwitchActivInt(SWITCH_1, FuncTecla1, 0);
	SwitchActivInt(SWITCH_2, FuncTecla2, 0);
	
	timer_config_t timer_medicion = {
        .timer = TIMER_A,
        .period = CONFIG_BLINK_PERIOD_MEDICION,
        .func_p = FuncTimerMedicion,
        .param_p = NULL
    };
    TimerInit(&timer_medicion);

	timer_config_t timer_radiacion = {
		.timer = TIMER_B,
		.period = CONFIG_BLINK_PERIOD_RADIACION,
		.func_p = FuncTimerRadiacion,
		.param_p = NULL
    };
    TimerInit(&timer_radiacion);

	serial_config_t my_uart = {
		.port = UART_PC,
		.baud_rate = 9600,
		.func_p = NULL,
		.param_p = NULL
	};
	UartInit(&my_uart);

	/* Creación de tareas */
	xTaskCreate(&MedicionRadiacionTask, "Medicion de radiacion", 512, NULL, 5, &radiacion_task_handle);
    xTaskCreate(&MedicionHumedadTemperaturaTask, "Medicion humedad temperatura", 512, NULL, 5, &medicion_task_handle);
	TimerStart(timer_medicion.timer);
	TimerStart(timer_radiacion.timer);

}
/*==================[end of file]============================================*/