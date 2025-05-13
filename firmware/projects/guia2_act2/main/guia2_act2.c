/*! @mainpage Actividad 2 de la guia 2
 *
 * @section se resuelve la actividad 2 
 *
 * En este ejercicio se modifica el ejercicio 1 agregando interrupciones para las teclas y los timers.
 *
 *
 *
 * @author Maria Victoria Boretto
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "led.h" 
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "switch.h"
#include "gpio_mcu.h"
#include "timer_mcu.h"
#include "hc_sr04.h" //sensor de ultrasonido
#include "lcditse0803.h" //lcd
/*==================[macros and definitions]=================================*/
#define CONFIG_BLINK_PERIOD_VISUALIZACION 1000000

uint16_t distancia;
bool hold = false;
bool encendido = true;
/*==================[internal data definition]===============================*/
TaskHandle_t medicion_visualizacion_task_handle = NULL;

/*==================[internal functions declaration]=========================*/
/**
 * @brief Función invocada en la interrupción del timer A
 */
void FuncTimerA(void* param){
    vTaskNotifyGiveFromISR(visualizacion_task_handle, pdFALSE);    /* Envía una notificación a la tarea asociada a visualizacion */
}

/**
 * @brief Función invocada en la interrupción de la tecla 1
 */
void FuncTecla1(void){
	encendido = !encendido;

}

/**
 * @brief Función invocada en la interrupción de la tecla 2
 */
void FuncTecla2(void){
	hold = !hold;
}

/**
 * @brief Tarea que se encarga de medir y mostrar la distancia en el LCD y ademas prender la cantidad de LEDs correspondientes a la medicion
 */
static void MedicionVisualizacionTask(void *pvParameter){
	while(true){
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		if(encendido){
			distancia = HcSr04ReadDistanceInCentimeters();
			LedsOffAll();
			if (distancia >= 10 && distancia < 20) {
				LedOn(LED_1);
				LedOff(LED_2);
				LedOff(LED_3);
			} else if (distancia >= 20 && distancia < 30) {
				LedOn(LED_1);
				LedOn(LED_2);
				LedOff(LED_3);
			} else if (distancia >= 30) {
				LedOn(LED_1);
				LedOn(LED_2);
				LedOn(LED_3);
			}
			if(!hold){
				//printf("mostrando distancia");
				LcdItsE0803Write(distancia);
			}
		}

		else{
			LcdItsE0803Off();
			LedsOffAll();
		}
		
	}
}
/*==================[external functions definition]==========================*/
void app_main(void){
	LedsInit();
	LcdItsE0803Init();
	HcSr04Init(GPIO_3, GPIO_2);
	SwitchesInit();
	SwitchActivInt(SWITCH_1, FuncTecla1, 0);
	SwitchActivInt(SWITCH_2, FuncTecla2, 0);
	
	timer_config_t timer_visualizacion = {
        .timer = TIMER_A,
        .period = CONFIG_BLINK_PERIOD_VISUALIZACION,
        .func_p = FuncTimerA,
        .param_p = NULL
    };
    TimerInit(&timer_visualizacion);

    xTaskCreate(&MedicionVisualizacionTask, "Medicion y Visualizacion LCD", 512, NULL, 5, &medicion_visualizacion_task_handle);

	TimerStart(timer_visualizacion.timer);
}
/*==================[end of file]============================================*/