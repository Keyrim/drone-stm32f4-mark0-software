/*
 * acc.h
 *
 *  Created on: 1 déc. 2020
 *      Author: Theo
 */

#ifndef SENSORS_ACC_H_
#define SENSORS_ACC_H_

#include "stm32f4xx_hal.h"
#include "sensors.h"
#include "mpu.h"

#define ACC_USE_MPU

typedef struct acc_t{
	sensor_state_e state ;

	//Available acc list
	mpu_t * mpu ;

	//Raw accelerations
	float * raw ;
	//filtered accelerations
	float filtered[3];
	//Filters
	Filter_t filters[3];

}acc_t;

sensor_state_e ACC_init(acc_t * acc, mpu_t * mpu);
sensor_state_e ACC_update(acc_t * acc);
void ACC_process_lpf(acc_t * acc);

#endif /* SENSORS_ACC_H_ */
