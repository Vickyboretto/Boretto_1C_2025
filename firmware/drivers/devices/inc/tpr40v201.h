#ifndef TPR40V201_H
#define TPR40V201_H
/** \addtogroup Drivers_Programable Drivers Programable
 ** @{ */
/** \addtogroup Drivers_Devices Drivers devices
 ** @{ */
/** \addtogroup TPR40-V201
 ** @{ */

/** @brief 
 *
 *
 * @author Maria Victoria Boretto
 * 
 * @section changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 22/05/2025 | Document creation		                         |
 *
 */

/*==================[inclusions]=============================================*/
#include <stdint.h>

/*==================[macros]=================================================*/
// Máscaras de gestos (pueden ser usadas como bits individuales)
#define GESTURE_UP      (1 << 0)
#define GESTURE_DOWN    (1 << 1)
#define GESTURE_FORWARD (1 << 2)
#define GESTURE_BACK    (1 << 3)

/*==================[typedef]================================================*/

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/

/**
 * @brief Gesture Trackpad inicialization
 * 
 */
void GesturetrackpadInit(void);

/**
 * @brief read gestures 
 * 
 * @return int8_t 
 */
int8_t GesturetrackpadRead(void);

/** @} doxygen end group definition */
/** @} doxygen end group definition */
/** @} doxygen end group definition */
#endif

/*==================[end of file]============================================*/
