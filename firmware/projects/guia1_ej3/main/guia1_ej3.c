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
/*==================[macros and definitions]=================================*/
#define CONFIG_BLINK_PERIOD 100 // defino el tiempo de toogle del led
#define ON 1
#define OFF 0
#define TOGGLE 2

/*==================[internal data definition]===============================*/

//defino la estructura de leds y enumero los leds que voy a utilizar
typedef struct 
{
    uint8_t mode;       //ON, OFF, TOGGLE
	uint8_t n_led;        //indica el número de led a controlar
	uint8_t n_ciclos;   //indica la cantidad de ciclos de ncendido/apagado
	uint16_t periodo;    //indica el tiempo de cada ciclo
} leds;
leds my_led1, my_led2, my_led3;

/*==================[internal functions declaration]=========================*/
void Funcion_leds(leds*my_leds){
	uint8_t retardo = my_leds->periodo/CONFIG_BLINK_PERIOD;
	printf("inicia funcion, retardo=%d\n", retardo);
	switch (my_leds->mode) 
	{
	case ON:
		LedOn(my_leds->n_led); //prende el led que se encuentra en el puntero n_led
		printf("prendo led\n");
		break;
	case OFF:
		LedOff(my_leds->n_led);
		printf("apago led\n");
		break;
	case TOGGLE:
		for(int i = 0; i<my_leds->n_ciclos; i++){
			LedToggle(my_leds->n_led); //parpadea el led que esta en n_led
			printf("parpadea led\n");
			for(int j = 0; j<retardo; j++){
				vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS); //siempre va asi por el sistema operativo
				printf("esperando\n");
			}
			
		}
		break;
	}
}

/*==================[external functions definition]==========================*/
void app_main(void){
	LedsInit();
	leds my_led1;
	leds my_led2;
	leds my_led3;
	my_led1.n_led = LED_1;
	my_led2.n_led = LED_2;
	my_led3.n_led = LED_3;
	my_led1.mode = ON;
	my_led2.mode = OFF;
	my_led3.mode = TOGGLE;
	my_led1.n_ciclos = 10;
	my_led2.n_ciclos = 10;
	my_led3.n_ciclos = 10;
	my_led1.periodo = 500;
	my_led2.periodo = 500;
	my_led3.periodo = 500;
	Funcion_leds(&my_led1);
	Funcion_leds(&my_led2);
	Funcion_leds(&my_led3);
	while (1){
		vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
	}
		
	
}
/*==================[end of file]============================================*/