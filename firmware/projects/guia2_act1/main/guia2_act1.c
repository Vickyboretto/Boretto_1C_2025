/*! @mainpage Template
 *
 * @section genDesc General Description
 *
 * This section describes how the program works.
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	PIN_X	 	| 	GPIO_X		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 12/09/2023 | Document creation		                         |
 *
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
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
#include "hc_sr04.h" //ultrasonido
#include "lcditse0803.h" //lcd
/*==================[macros and definitions]=================================*/
#define CONFIG_BLINK_PERIOD_SENSOR 1000
#define CONFIG_BLINK_PERIOD_TECLAS 100
#define CONFIG_BLINK_PERIOD_VISUALIZACION 1000

uint16_t distancia;
bool hold = false;
bool encendido = true;

/*==================[internal data definition]===============================*/
TaskHandle_t medir_task_handle = NULL;
TaskHandle_t teclas_task_handle = NULL;
TaskHandle_t visualizacion_task_handle = NULL;

/*==================[internal functions declaration]=========================*/

static void MedirTask(void *pvParameter){
    while(true){
        distancia = HcSr04ReadDistanceInCentimeters();
        vTaskDelay(CONFIG_BLINK_PERIOD_SENSOR / portTICK_PERIOD_MS);

    }
}

static void TeclasTask(void *pvParameter){
	while(true){
		uint8_t teclas = SwitchesRead();

        if (teclas & SWITCH_1) {  // TEC1: Start/Stop medición
            encendido = !encendido;
        }

        if (teclas & SWITCH_2) {  // TEC2: HOLD
            hold = !hold;
        }

        vTaskDelay(CONFIG_BLINK_PERIOD_TECLAS / portTICK_PERIOD_MS);
	}
	
}

static void VisualizacionTask(void *pvParameter){
	while(true){
		if(encendido){
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
		vTaskDelay(CONFIG_BLINK_PERIOD_VISUALIZACION / portTICK_PERIOD_MS);
	}
}

/*==================[external functions definition]==========================*/
void app_main(void){
	LedsInit();
	LcdItsE0803Init();
	HcSr04Init(GPIO_3, GPIO_2);
	SwitchesInit();

	xTaskCreate(&MedirTask, "Medir", 512, NULL, 5, &medir_task_handle);
    xTaskCreate(&TeclasTask, "Teclas", 512, NULL, 5, &teclas_task_handle);
    xTaskCreate(&VisualizacionTask, "Visualizacion LCD", 512, NULL, 5, &visualizacion_task_handle);

}
/*==================[end of file]============================================*/