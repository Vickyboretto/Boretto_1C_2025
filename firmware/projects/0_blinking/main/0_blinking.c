/*! @mainpage Blinking
 *
 * \section genDesc General Description
 *
 * This example makes LED_1 blink.
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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "tpr40v201.h"
#include "timer_mcu.h"
#include "gpio_mcu.h"
/*==================[macros and definitions]=================================*/
#define CONFIG_BLINK_PERIOD 1000
#define SAMPLE_PERIOD_US 2000 // 2 ms



/*==================[internal data definition]===============================*/
TaskHandle_t read_gestures_task_handle = NULL;

/*==================[internal functions declaration]=========================*/
/**
 * @brief Función invocada en la interrupción del timer A
 */
void FuncTimerA(void* param){
    vTaskNotifyGiveFromISR(read_gestures_task_handle, pdFALSE);    /* Envía una notificación a la tarea asociada a read gestures */
}

void read_gesturesTask(void *pvParameter){
    //GesturetrackpadInit();
     
    while(true){
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        int8_t gesture = GesturetrackpadRead(); 
        //printf("leyendo gestos");
        switch (gesture) {
            case GESTURE_UP:
                LedOn(LED_1);
                break;
            case GESTURE_DOWN:
                LedOff(LED_1);
                break;
            case GESTURE_FORWARD:
                LedOn(LED_2);
                break;
            case GESTURE_BACK:
                LedOff(LED_2);
                break;
            default:
                break;
    }
    }
}

/*==================[external functions definition]==========================*/
void app_main(void){
    
    timer_config_t timer_config_a = {
        .timer = TIMER_A,
        .period = SAMPLE_PERIOD_US, 
        .func_p = FuncTimerA,
        .param_p = NULL
	};
	TimerInit(&timer_config_a);
    LedsInit();
    LedsOffAll();
    GesturetrackpadInit();

    xTaskCreate(&read_gesturesTask, "Read gestures", 512, NULL, 5, &read_gestures_task_handle);
    TimerStart(TIMER_A);
    /*
    while(true){
           if (GPIORead(GPIO_0))
            LedOn(LED_1);
        if (GPIORead(GPIO_2))
            LedOff(LED_1);
        if (GPIORead(GPIO_1))
            LedOn(LED_2);
        if (GPIORead(GPIO_1))
            LedOn(LED_3);
        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
    }*/
}
/*==================[end of file]============================================*/
