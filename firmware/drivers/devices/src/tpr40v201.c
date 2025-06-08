/**
 * @file tpr40v201.c
 * @author Maria Victoria Boretto 
 * @brief 
 * @version 0.1
 * @date 2025-05-22
 * 
 * @copyright Copyright (c) 2025
 * 
 */

/*==================[inclusions]=============================================*/
#include "tpr40v201.h"
#include "gpio_mcu.h"
/*==================[macros and definitions]=================================*/
#define GPIO_GESTURE_UP GPIO_2  //I/O 1
#define GPIO_GESTURE_DOWN GPIO_3  //I/O 3
#define GPIO_GESTURE_FORWARD GPIO_1  //I/O 5
#define GPIO_GESTURE_BACK GPIO_0  //I/O 4


/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/
void GesturetrackpadInit(void){
	/* GPIO configurations */
	GPIOInit(GPIO_GESTURE_UP, GPIO_INPUT);	// GESTURE_UP
	GPIOInit(GPIO_GESTURE_DOWN, GPIO_INPUT);	// GESTURE_DOWN
	GPIOInit(GPIO_GESTURE_FORWARD, GPIO_INPUT);   // GESTURE_TAP
	GPIOInit(GPIO_GESTURE_BACK, GPIO_INPUT);   // GESTURE_HOLD
    
    }

int8_t GesturetrackpadRead(void){
    //int8_t gesture_mask = 0;

    if (GPIORead(GPIO_GESTURE_UP))
        return GESTURE_UP;
    if (GPIORead(GPIO_GESTURE_DOWN))
        return GESTURE_DOWN;
    if (GPIORead(GPIO_GESTURE_FORWARD))
        return GESTURE_FORWARD;
    if (GPIORead(GPIO_GESTURE_BACK))
        return GESTURE_BACK;

    return 0; // No se detectó gesto
}
    
/*==================[end of file]============================================*/
