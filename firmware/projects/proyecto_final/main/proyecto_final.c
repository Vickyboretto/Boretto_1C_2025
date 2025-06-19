/*! @mainpage Proyecto Final
 *
 * @section genDesc General Description
 *
 * Este proyecto ejemplifica el uso del módulo de comunicación Bluetooth Low Energy (BLE) 
 * junto con el manejo de tiras de LEDs RGB. 
 * Permite manejar mediante un Gesture Trackpad el encendido y apagado de la tira neopixel, ademas de 
 * manejar la intensidad de los LEDs.
 * Y, por medio de bluetooth se puede elegir un color o un modo (arcoiris o respiracion).
 * 
 * @section Neopixel Stripe Connection
 *
 * |  ESP32 Pin     | Neopixel Pin 	|
 * |:--------------:|:--------------|
 * |   GND  	 	| 	 GND		|
 * |   VCC     	    |    5V     	| 
 * |  DATA INPUT   	|    GPIO16    	| 
 * 
 * @section Gesture Trackpad Connection
 * 
 * |  ESP32 Pin     | Gesture Trackpad Pin |
 * |:--------------:|:---------------------|
 * |     GND  	 	|         GND	       |
 * |     VCC   	    |         3V3          | 
 * |    I/O 4   	|         GPIO_0       | 
 * |    I/O 5   	|         GPIO_1       | 
 * |    I/O 1   	|         GPIO_2       | 
 * |    I/O 3   	|         GPIO_3       | 
 * 
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

uint8_t red = 255, green = 255, blue = 255;  // inicializo todos los leds en blanco
uint8_t brillo;  // Brillo como porcentaje (0-255)
bool neopixel_encendido = true;  // comienza apagado
bool tecla_arcoiris = false;
bool tecla_respiracion = false;


/*==================[internal data definition]===============================*/
TaskHandle_t read_gestures_task_handle = NULL;


/*==================[internal functions declaration]=========================*/
/**
 * @brief Función invocada en la interrupción del timer A
 */
void FuncTimerA(void* param){
    vTaskNotifyGiveFromISR(read_gestures_task_handle, pdFALSE);    /* Envía una notificación a la tarea asociada a read gestures */
}
/**
 * @brief Función que realiza el modo arcoiris
 */
void RainbowEffect(void){
    uint16_t hue = 0;
    uint16_t ciclo = 150;
    while (ciclo > 0){
        NeoPixelRainbow(hue,255,255,1);
        hue += 256;
        ciclo -= 1;
        vTaskDelay(pdMS_TO_TICKS(12)); 
        
    }
}
/**
 * @brief Función que realiza el modo respiracion
 */
void RespiracionEffect(void){
    uint16_t brillo;
    uint16_t ciclo = 2;
    while(ciclo > 0){
        for (brillo = 255; brillo > 0; brillo -= 3) {
            NeoPixelAllColor(NeoPixelRgb2Color((brillo),
                                                (brillo),
                                                (brillo) ));
            printf("bajando brillo: %d\r\n", brillo);
            vTaskDelay(30 / portTICK_PERIOD_MS);
        }
        for (brillo = 0; brillo <= 255; brillo += 3) {
            NeoPixelAllColor(NeoPixelRgb2Color((brillo),
                                                    (brillo) ,
                                                    (brillo) ));
            printf("subiendo brillo: %d\r\n", brillo);
            vTaskDelay(30 / portTICK_PERIOD_MS);
    }
    ciclo -= 1;
    }
    
    
}
/**
 * @brief Tarea que se encarga de leer los gestos que se realizan en el Gesture Trackpad
 */
void read_gesturesTask(void *pvParameter){
    while(true){
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        int8_t gesture = GesturetrackpadRead();
        switch (gesture) {
            case GESTURE_UP:
                if (brillo < 215) {
                    printf("brillo: %d \n",  brillo);
                    brillo += 40;  
                    printf("brillo: %d \n",brillo);
                if (brillo > 255) brillo = 255;  
                }
                if (neopixel_encendido) {
                    NeoPixelAllColor(NeoPixelRgb2Color(
                        (red * brillo) / 255,
                        (green * brillo) / 255,
                        (blue * brillo) / 255));
                }
                vTaskDelay(500 / portTICK_PERIOD_MS);
                break;
            case GESTURE_DOWN:
                if (brillo > 40){
                    printf("brillo: %d \n",brillo);
                    brillo -= 40;
                    printf("brillo: %d \n",brillo);
                    NeoPixelAllColor(NeoPixelRgb2Color((red * brillo) / 255,(green * brillo) / 255,(blue * brillo) / 255));
                } 
                if (brillo == 0){
                    break;
                } 
                if (neopixel_encendido){
                    NeoPixelAllColor(NeoPixelRgb2Color((red * brillo) / 255,(green * brillo) / 255,(blue * brillo) / 255));
                }
                vTaskDelay(500 / portTICK_PERIOD_MS);
                break;
            case GESTURE_FORWARD:
                neopixel_encendido = true;
                NeoPixelAllColor(NeoPixelRgb2Color(red, green, blue));  //enciende todos los leds de blanco con el brillo al maximo
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
 * @brief Función a ejecutarse ante una interrupción de recepción de datos a través de la conexión BLE.
 * 
 * @param data      Puntero a array de datos recibidos
 * @param length    Longitud del array de datos recibidos
 */
void read_data(uint8_t * data, uint8_t length){  // interrupcion desde el bluetooth, para cambiar un color o elegir un modo
	uint8_t i = 1;
	char msg[30];
    bool rainbow_mode = false;
	if(data[0] == 'R'){
		red = 0;
		while(data[i] != 'A'){
			red = red * 10;
			red = red + (data[i] - '0');
			i++;
		}
	}else if(data[0] == 'G'){   
		green = 0;
		while(data[i] != 'A'){
			green = green * 10;
			green = green + (data[i] - '0');
			i++;
		}
	}else if(data[0] == 'B'){
		blue = 0;
		while(data[i] != 'A'){
			blue = blue * 10;
			blue = blue + (data[i] - '0');
			i++;
		}
	}else if(data[0] == 'C'){
        tecla_arcoiris = true;
        RainbowEffect();
	}else if(data[0] == 'Z'){
        tecla_respiracion = true;
        RespiracionEffect();
	}if (!rainbow_mode) {
		NeoPixelAllColor(NeoPixelRgb2Color(red, green, blue));
    }if (!tecla_respiracion) {
		NeoPixelAllColor(NeoPixelRgb2Color(red, green, blue));
	}
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
    NeoPixelInit(BUILT_IN_RGB_LED_PIN, LED_LENGTH, color);
    GesturetrackpadInit();   
    xTaskCreate(&read_gesturesTask, "Read gestures", 2048, NULL, 5, &read_gestures_task_handle);
    TimerStart(TIMER_A);

    while(1){  
        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
        switch(BleStatus()){   //dependiendo del estado del bluetooth se apaga, prende o togglea el led
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
