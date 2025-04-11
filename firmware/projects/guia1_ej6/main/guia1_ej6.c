/*! @mainpage Ejercicio 6 de la guia 1
 *
 * @section se realiza el ejercicio 6
 *
 * El ejercicio consiste en darle un valor a data, el cual es covertido a BCD, 
 * luego el BCD es utilizado para escribir el numero en los gpio, y cada gpio es enviado
 * a su vez a los LCD, mostrando en la pantalla el numero ingresado en data.
 *
 *
 *
 * @author Maria Victoria Boretto (vickybore00@gmail.com)
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
/**
 *  @brief defino los GPIO que voy a utilizar y le doy sus valores correspondientes 
**/

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

/** @fn converttoBCD
 * @brief Funcion que convierte data (numero) a BCD
 * @param data Valor decimal a mostrar
 * @param digits Cantidad de digitos
 * @param bcdArray Arreglo de BCD
 * @return valor de -1 si no se ingreso un numero o si el numero ingresado tiene mas de 10 digitos, 
 * 			y valor de 0 si el numero ingresado fue convertido a BCD
 *
 */
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

/** @fn BCDtoGPIO
 * @brief Funcion que utiliza el data en BCD para apagar o prender los GPIO (mostrando los dígitos)
 * @param digit Digitos del numero
 * @param gpio_config Arreglo de 4 estructuras `gpioConf_t` que contiene la configuración 
 *                    de cada pin GPIO (pin y dirección).
 * @return 
 *
 */
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

/** @fn GPIOtoLCD 
 * @brief Funcion que muestra los digitos del numero ingresado en el LCD
 * @param data Valor decimal a mostrar.
 * @param digits Cantidad de dígitos del display.
 * @param gpio_digit Arreglo de configuración de pines para cada dígito del display.
 * @param gpio_map Mapeo de pines GPIO para cada bit BCD.
 * @return
 *
 */
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
	uint32_t data = 587;   //inicializamos los numeros y la cantidad de digitos, esto podria ingresarse por consola para que no quede hardcodeado
    uint8_t digits = 3;
    
	GPIOtoLCD(data, digits, gpio_digit, gpio_map);
	
	
}
/*==================[end of file]============================================*/