/*! @mainpage Proyecto Final
 *
 * @section genDesc General Description
 *
 * Este proyecto ejemplifica el uso del módulo de comunicación Bluetooth Low Energy (BLE) 
 * junto con el manejo de tiras de LEDs RGB. 
 * Permite manejar la tonalidad e intensidad del LED RGB incluído en la placa ESP-EDU, 
 * mediante una aplicación móvil.
 *
 * @author Maria Victoria Boretto (vickybore00@gmail.com)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "timer_mcu.h"
#include "led.h"
#include "neopixel_stripe.h"
#include "ble_mcu.h"
#include "tpr40v201.h"
/*==================[macros and definitions]=================================*/
#define CONFIG_BLINK_PERIOD 500
#define LED_BT	LED_1
#define LED_LENGTH 12 
#define SAMPLE_PERIOD_US 2000 // 2 ms

uint8_t red = 255, green = 255, blue = 255;  // color blanco inicial
uint8_t brillo = 100;  // Brillo como porcentaje (0-255)
uint8_t neopixel_encendido = true;  // comienza apagado

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
    
    while(true){
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        int8_t gesture = GesturetrackpadRead();
        switch (gesture) {
            case GESTURE_UP:
                if (brillo < 255) {
                    printf("brillo: %d",  brillo);
                    brillo += (uint8_t)(255 * 0.10);  // Aumenta un 10%
                    printf("brillo: %d",brillo);
                if (brillo > 255) brillo = 255;  // Limita a 255 máximo
                }
                if (neopixel_encendido) {
                    NeoPixelAllColor(NeoPixelRgb2Color(
                        (red * brillo) / 255,
                        (green * brillo) / 255,
                        (blue * brillo) / 255));
                }
                break;
            case GESTURE_DOWN:
                if (brillo > 0){
                    printf("brillo: %d",brillo);
                    brillo -= 10;
                    printf("brillo: %d",brillo);
                } 
                if (brillo == 0){
                    break;
                } 
                if (neopixel_encendido){
                    NeoPixelAllColor(NeoPixelRgb2Color((red * brillo) / 255,(green * brillo) / 255,(blue * brillo) / 255));
                }
                break;
            case GESTURE_FORWARD:
                neopixel_encendido = true;
                NeoPixelAllColor(NeoPixelRgb2Color(255, 255, 255));  //enciende todos los leds de blanco con el brillo al maximo
                break;
            case GESTURE_BACK:
                neopixel_encendido = false;
                NeoPixelAllOff();  //apago la tira led
                break;
            default:
                break;
    }
    }
}


/**
 * @brief Función a ejecutarse ante un interrupción de recepción 
 * a través de la conexión BLE.
 * 
 * @param data      Puntero a array de datos recibidos
 * @param length    Longitud del array de datos recibidos
 */
void read_data(uint8_t * data, uint8_t length){  // interrupcion desde el bluetooth, para cambiar un color por ejemplo
	uint8_t i = 1;
	char msg[30];

	if(data[0] == 'R'){
        /* El slidebar Rojo envía los datos con el formato "R" + value + "A" */
		red = 0;
		while(data[i] != 'A'){
            /* Convertir el valor ASCII a un valor entero */
			red = red * 10;
			red = red + (data[i] - '0');
			i++;
		}
	}else if(data[0] == 'G'){   
        /* El slidebar Verde envía los datos con el formato "G" + value + "A" */
		green = 0;
		while(data[i] != 'A'){
            /* Convertir el valor ASCII a un valor entero */
			green = green * 10;
			green = green + (data[i] - '0');
			i++;
		}
	}else if(data[0] == 'B'){
        /* El slidebar Azul envía los datos con el formato "B" + value + "A" */
		blue = 0;
		while(data[i] != 'A'){
            /* Convertir el valor ASCII a un valor entero */
			blue = blue * 10;
			blue = blue + (data[i] - '0');
			i++;
		}
	}
    NeoPixelAllColor(NeoPixelRgb2Color(red, green, blue));
    /* Se envía una realimentación de los valores actuales de brillo del LED */
    sprintf(msg, "R: %d, G: %d, B: %d\n", red, green, blue);
    BleSendString(msg);
}

/*==================[external functions definition]==========================*/
void app_main(void){
    static neopixel_color_t color[12];

    timer_config_t timer_config_a = {
    .timer = TIMER_A,
    .period = SAMPLE_PERIOD_US, 
    .func_p = FuncTimerA,
    .param_p = NULL
	};
	TimerInit(&timer_config_a);

    ble_config_t ble_configuration = {
        "ESP_EDU_MVB",
        read_data  // funcion de callback para cambiar el color desde el bluetooth
    };

    LedsInit();
    BleInit(&ble_configuration);
    /* Se inicializa el LED RGB de la placa */
    NeoPixelInit(BUILT_IN_RGB_LED_PIN, LED_LENGTH, color);
    GesturetrackpadInit();   // inicializo el GT
   
    xTaskCreate(&read_gesturesTask, "Read gestures", 2048, NULL, 5, &read_gestures_task_handle);
    TimerStart(TIMER_A);

    while(1){  
        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
        switch(BleStatus()){   //dependiendo del estado del bluetooth se apagan, prenden o togglea el led
            case BLE_OFF:
                LedOff(LED_BT);
            break;
            case BLE_DISCONNECTED:
                LedToggle(LED_BT);
            break;
            case BLE_CONNECTED:
                LedOn(LED_BT);
            break;
        }
    }
    


}

/*==================[end of file]============================================*/
