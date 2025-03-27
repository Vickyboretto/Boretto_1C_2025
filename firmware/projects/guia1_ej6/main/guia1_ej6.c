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

/*==================[macros and definitions]=================================*/
#define GPIO_20    20
#define GPIO_21    21
#define GPIO_22    22
#define GPIO_23    23
#define GPIO_19    19
#define GPIO_18    18
#define GPIO_9     9

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
gpioConf_t gpio_digit[3] = {
    {GPIO_19, 1}, // b0 -> GPIO_19
    {GPIO_18, 1}, // b1 -> GPIO_18
    {GPIO_9, 1}   // b2 -> GPIO_9
};

/*==================[internal functions declaration]=========================*/
// convierte el numero a bcd
int8_t convertToBCD(uint32_t data, uint8_t digits, uint8_t *bcdArray) {
	if (digits > 10){
		return -1; //devuelve -1 porque el valor tiene mas de 10 digitos
	}
//se encarga de la conversion a bcd
    for (int i = digits - 1; i >= 0; i--) {
        bcdArray[i] = data % 10;  // Extrae el último dígito
        data /= 10;       		// Elimina el dígito extraído
		       
    }
	return 0; 
	
	if (digits < 0){
		return -1; //devuelve -1 porque no hay un numero
	}
}
//usando el binario se prenden o apagan los gpio
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
 void GPIOtoLCD(uint32_t data, uint8_t digits, gpioConf_t *gpio_digit, gpioConf_t *gpio_map){
	uint8_t bcdArray[digits];
	uint8_t resultado = convertToBCD(data, digits, bcdArray);

	if (resultado == 0){ //se separo en digitos
		for (uint8_t i = 0; i < 3; i++) {
			GPIOInit(gpio_digit[i].pin, gpio_digit[i].dir);
		}
		for (uint8_t i = 0; i < 3; i++){
			BCDtoGPIO(bcdArray[i], gpio_map);
			GPIOOn(gpio_digit[i].pin);
			GPIOOff(gpio_digit[i].pin);
		}
		

	}

 }


/*==================[external functions definition]==========================*/
void app_main(void){
	uint32_t data = 587;   //inicializamos los numeros, digitos y el arreglo con un valor que querramos
    uint8_t digits = 3;
    //uint8_t bcdArray[8] = {0}; // Inicializamos en 0

	GPIOtoLCD(data, digits, gpio_digit, gpio_map);
	
	
}
/*==================[end of file]============================================*/