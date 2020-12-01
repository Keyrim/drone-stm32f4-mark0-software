/*
 * system_d.h
 *
 *  Created on: Nov 21, 2020
 *      Author: Théo
 */

#ifndef OS_SYSTEM_D_H_
#define OS_SYSTEM_D_H_

#include "macro_types.h"
#include "stm32f4xx_hal.h"
#include "../Sensors/gyro.h"
#include "../../Drivers/Inc/Sequence_led.h"

typedef struct sensors_t{
	//Sensor type
	gyro_t gyro ;

	//Sensors
	mpu_t mpu;
}sensors_t;

typedef struct ihm_t{
	sequence_led_t led_red ;
	sequence_led_t led_green ;
	sequence_led_t led_blue ;
}ihm_t;

typedef struct{
	ihm_t ihm ;
	sensors_t sensors ;
}system_t;


#endif /* OS_SYSTEM_D_H_ */
