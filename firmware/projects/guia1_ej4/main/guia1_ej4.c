/*! @mainpage Ejercicio 4 de la guia 1
 *
 * @section se realiza el ejercicio 4
 *
 * En este ejercicio se convierte el numero decimal ingresado a BCD y guardando cada digito en un arreglo.
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 12/09/2023 | Document creation		                         |
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
		for (int i = 0; i < digits; i++) {
			printf("%d ", bcdArray[i]);   //muestra el valor separado por digitos
		}
	}
	else {
		printf("Conversion fallida");
	}
}
/*==================[end of file]============================================*/