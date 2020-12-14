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
#include "../Sensors/acc.h"
#include "../Sensors/ms5611.h"
#include "../Estimators/orientation.h"
#include "../Radio/controller.h"
#include "../Propulsion/Propulsion.h"
#include "../Regulation/Regu_orientation.h"
#include "../../Drivers/Inc/Sequence_led.h"
#include "../../Drivers/Inc/Ibus.h"
#include "../../Drivers/Inc/Motors.h"
#include "../config.h"

typedef struct sensors_t{
	//Sensors
	gyro_t gyro ;
	acc_t acc ;


	//Sensors list
	mpu_t mpu ;
	ms5611_t ms5611 ;
}sensors_t;

typedef struct regulation_t{
	//Orientation
	regu_orientation_t orientation ;
	//Position
	//Todo position regulation
}regulation_t;



typedef struct radio_t{
	ibus_t ibus;
	controller_t controller ;
}radio_t;

typedef struct ihm_t{
	sequence_led_t led_red ;
	sequence_led_t led_green ;
	sequence_led_t led_blue ;
}ihm_t;

typedef struct{
	ihm_t ihm ;
	sensors_t sensors ;
	orientation_t orientation ;
	radio_t	radio;
	propulsion_t propulsion ;
	regulation_t regulation ;
}system_t;


#endif /* OS_SYSTEM_D_H_ */
