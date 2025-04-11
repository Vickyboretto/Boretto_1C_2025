/*! @mainpage Ejercicio 2 de la guia 1
 *
 * @section se resuelve el ejercicio 2
 *
 * En este ejercicio se va a hacer titilar los leds 1 y 2 al apretar las teclas 1 y 2 respectivamente, y si se aprietan ambas va a titilar el led 3.
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 12/09/2023 | Document creation		                         |
 *
 * @author MAria Victoria Boretto
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "switch.h"
/*==================[macros and definitions]=================================*/
#define CONFIG_BLINK_PERIOD 100

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/
void app_main(void){
	uint8_t teclas;
	bool flagswitch1 = false;
	bool flagswitch2 = false;
	LedsInit();
	SwitchesInit();
    while(1)    {
    	teclas  = SwitchesRead();
    	switch(teclas){
    		case SWITCH_1:
    			flagswitch1 = !flagswitch1;
    		break;
    		case SWITCH_2:
    			flagswitch2 = !flagswitch2;
    		break;
		}
		if(flagswitch1 && !flagswitch2){
			LedToggle(LED_1);
			
		}
		else if(flagswitch2 && !flagswitch1){
			LedToggle(LED_2);
		
		}
		else if(flagswitch1 && flagswitch2){
			LedToggle(LED_1);
			LedToggle(LED_2);
		
		}
	    //LedToggle(LED_3);
		vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
	}
}
/*==================[end of file]============================================*/