/*! @mainpage Actividad 3 de la guia 2
 *
 * @section se resuelve la actividad 3
 *
 * En esta actividad se modifica la actividad 2, mostrando la distancia medida en la PC, con posibilidad
 * de controlar la EDU_ESP desde la PC para prender o apagar el LCD, holdear, mostrar la maxima distancia medida y 
 * mostrar la distancia en pulgadas.
 *
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
#include "gpio_mcu.h"
#include "timer_mcu.h"
#include "hc_sr04.h" //sensor de ultrasonido
#include "lcditse0803.h" //lcd
#include "uart_mcu.h"
/*==================[macros and definitions]=================================*/
#define CONFIG_BLINK_PERIOD_VISUALIZACION 1000000

uint16_t distancia;
uint16_t distancia_maxima = 0;
bool hold = false;
bool encendido = true;
bool pulgadas = false;
bool maximo = false;
/*==================[internal data definition]===============================*/
TaskHandle_t medicion_visualizacion_task_handle = NULL;
TaskHandle_t uart_task_handle = NULL;
/*==================[internal functions declaration]=========================*/
/**
 * @brief Función invocada en la interrupción del timer A
 */
void FuncTimerA(void* param){
    vTaskNotifyGiveFromISR(medicion_visualizacion_task_handle, pdFALSE);    /* Envía una notificación a la tarea asociada a visualizacion */
}

/**
 * @brief Función invocada en la interrupción de la tecla 1
 */
void FuncTecla1(void){
	encendido = !encendido;

}

/**
 * @brief Función invocada en la interrupción de la tecla 2
 */
void FuncTecla2(void){
	hold = !hold;
}

/**
 * @brief Función invocada en la interrupción del UART
 */
void FuncUart(void *pvParameter){
    uint8_t c;
    while(true){
        if (UartReadByte(UART_PC, &c)) {
            switch(c) {
                case 'O':   //se enciende o apaga el lcd, reseteando la distancia maxima medida
                    encendido = !encendido;
					distancia_maxima = 0;
                    break;
                case 'H':   //holdea el valor de distancia medido
                    hold = !hold;
                    break;
				case 'I':   //se muestra la distancia en pulgadas
					pulgadas = !pulgadas;
					break;
				case 'M':     //se muestra la maxima distancia medida
					maximo = !maximo;
					break;
			}
		}
	}
}

/**
 * @brief Tarea que se encarga de medir y mostrar la distancia (en cm o in) en el LCD, prender la cantidad de LEDs 
 * correspondientes a la medicion y mostrar el valor por UART PC
 */
static void MedicionVisualizacionTask(void *pvParameter){
	while(true){
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		if(encendido){
			if(pulgadas){
				distancia = HcSr04ReadDistanceInInches();
			}
			else{
				distancia = HcSr04ReadDistanceInCentimeters();
			}
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
			if(distancia > distancia_maxima){
				distancia_maxima = distancia;
			}
			if(!hold){
				LcdItsE0803Write(distancia);
				if(!maximo){
					UartSendString(UART_PC, (char*)UartItoa(distancia_maxima, 10));
				} else{
					UartSendString(UART_PC, (char*)UartItoa(distancia, 10));
				}
				
				if(pulgadas){
					UartSendString(UART_PC, " pulgadas\r\n");
				} else{
					UartSendString(UART_PC, " cm\r\n");
				}
				
			}
			
		}

		else{
			LcdItsE0803Off();
			LedsOffAll();
		}
		
	}
}


/*==================[external functions definition]==========================*/
void app_main(void){
	LedsInit();
	LcdItsE0803Init();
	HcSr04Init(GPIO_3, GPIO_2);
	SwitchesInit();
	SwitchActivInt(SWITCH_1, FuncTecla1, 0);
	SwitchActivInt(SWITCH_2, FuncTecla2, 0);
	
	timer_config_t timer_visualizacion = {
        .timer = TIMER_A,
        .period = CONFIG_BLINK_PERIOD_VISUALIZACION,
        .func_p = FuncTimerA,
        .param_p = NULL
    };
    TimerInit(&timer_visualizacion);

	serial_config_t my_uart = {
		.port = UART_PC,
		.baud_rate = 9600,
		.func_p = FuncUart,
		.param_p = NULL
	};
	UartInit(&my_uart);

	
    xTaskCreate(&MedicionVisualizacionTask, "Medicion y Visualizacion LCD", 512, NULL, 5, &medicion_visualizacion_task_handle);

	TimerStart(timer_visualizacion.timer);
}
/*==================[end of file]============================================*/