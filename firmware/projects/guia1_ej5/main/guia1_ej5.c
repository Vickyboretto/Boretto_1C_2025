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
/*Escribir una función que reciba como parámetro un dígito BCD y un vector de estructuras del tipo gpioConf_t. 
Incluya el archivo de cabecera gpio_mcu.h*/


/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "led.h" 
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "switch.h"
#include "gpio_mcu.h"
/*==================[macros and definitions]=================================*/
#define GPIO_20    20
#define GPIO_21    21
#define GPIO_22    22
#define GPIO_23    23

/*==================[internal data definition]===============================*/
typedef struct
{
	gpio_t pin;			/*!< GPIO pin number */
	io_t dir;			/*!< GPIO direction '0' IN;  '1' OUT*/
} gpioConf_t;

// Vector de estructuras para mapear los bits del BCD a los GPIO
gpioConf_t gpio_map[4] = {
    {GPIO_20, 1}, // b0 -> GPIO_20
    {GPIO_21, 1}, // b1 -> GPIO_21
    {GPIO_22, 1}, // b2 -> GPIO_22
    {GPIO_23, 1}  // b3 -> GPIO_23
};
/*==================[internal functions declaration]=========================*/
void BCDtoGPIO(uint8_t digit, gpioConf_t *gpio_config) {
    for (uint8_t i = 0; i < 4; i++) {
		GPIOInit(gpio_config[i].pin, gpio_config[i].dir);
	
        if ((digit & (1 << i)) == 0) {   //operacion AND de cada valor, va corriendo el 1 de posicion para multiplicar todos y ver si esta on o off
            GPIOOff(gpio_config[i].pin);  // Apagar el GPIO si el bit es 0
			printf("Valor de 0, gpio apagado");
        } else {
            GPIOOn(gpio_config[i].pin);   // Encender el GPIO si el bit es 1
			printf("Valor de 1, gpio prendido");
        }
    }
}

/*==================[external functions definition]==========================*/
void app_main(void){
	int8_t digit = 1;

	BCDtoGPIO(digit, gpio_map);
	
}
/*==================[end of file]============================================*/