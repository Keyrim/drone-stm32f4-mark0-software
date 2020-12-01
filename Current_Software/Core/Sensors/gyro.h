/*
 * gyro.h
 *
 *  Created on: Nov 22, 2020
 *      Author: Théo
 */

#ifndef SENSORS_GYRO_H_
#define SENSORS_GYRO_H_


#include "stm32f4xx_hal.h"
#include "sensors.h"
#include "mpu.h"

#define USE_GYRO_MPU	//Gyro currently used


typedef struct gyro_t{
	//
	sensor_connectivity_e connectivity ;
	sensor_state_e state ;

	//Available gyro list
	mpu_t * mpu ;

	//Raw angles
	float * raw ;
	//Filtered angles
	float filtered[3];
	//Filters
	//Todo : filters

}gyro_t;

sensor_state_e GYRO_init(gyro_t * gyro, mpu_t *mpu);
sensor_state_e GYRO_update(gyro_t * gyro);
sensor_state_e GYRO_update_dma(gyro_t * gyro);
void GYRO_dma_done(gyro_t * gyro);


#endif /* SENSORS_GYRO_H_ */
