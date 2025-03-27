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

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/
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


/*==================[external functions definition]==========================*/
void app_main(void){
	uint32_t data = 587;   //inicializamos los numeros, digitos y el arreglo con un valor que querramos
    uint8_t digits = 3;
    uint8_t bcdArray[8] = {0}; // Inicializamos en 0

	int8_t resultado = convertToBCD(data, digits, bcdArray); //se le asigna a resultado el valor de 0 o -1 si se pudo convertir o no el numero
	// al llamar la funcion se le asignan los digitos al arreglo BCD


    if (resultado == 0){
		printf("Conversion exitosa");
		printf("\n");
		//printf("BCD Representation: ");
		for (int i = 0; i < digits; i++) {
			printf("%d ", bcdArray[i]);   //muestra el valor separado por digitos
		}
		
	}
	else {
		printf("Conversion fallida");
	}
}
/*==================[end of file]============================================*/