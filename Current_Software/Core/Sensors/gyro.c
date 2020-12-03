/*
 * gyro.c
 *
 *  Created on: Nov 22, 2020
 *      Author: Théo
 */


#include "gyro.h"
#include "../OS/events/events.h"

float filter[3] = {0.1f, 0.9f, 0};

sensor_state_e GYRO_init(gyro_t * gyro, mpu_t * mpu){

	FILTER_init(&gyro->filters[GYRO_AXE_X], filter, FILTER_FIRST_ORDER);
	FILTER_init(&gyro->filters[GYRO_AXE_Y], filter, FILTER_FIRST_ORDER);
	FILTER_init(&gyro->filters[GYRO_AXE_Z], filter, FILTER_FIRST_ORDER);

#ifdef GYRO_USE_MPU
	gyro->mpu = mpu ;
	gyro->raw = gyro->mpu->gyro ;
	gyro->state = MPU_init_gyro(gyro->mpu, MPU_GYRO_2000s);
#else
#warning gyro not defined
	gyro->state = SENSOR_ERROR ;
#endif

	if(gyro->state == SENSOR_IDDLE)
		EVENT_Set_flag(FLAG_GYRO_INIT);

	return gyro->state ;
}

sensor_state_e GYRO_update(gyro_t * gyro){
	if(gyro->state == SENSOR_IDDLE){
#ifdef GYRO_USE_MPU
		gyro->state = MPU_update_gyro(gyro->mpu);
#else
#warning gyro not defined
		gyro->state = SENSOR_ERROR ;
#endif
	}


	return gyro->state ;
}

sensor_state_e GYRO_update_dma(gyro_t * gyro){

#ifdef GYRO_USE_MPU
	gyro->state = MPU_update_gyro_dma(gyro->mpu);
#else
#warning gyro not defined
	gyro->state = SENSOR_ERROR ;
#endif

	return gyro->state ;
}

void GYRO_process_lpf(gyro_t * gyro){
	gyro->filtered[GYRO_AXE_X] =  FILTER_process(&gyro->filters[GYRO_AXE_X], gyro->raw[GYRO_AXE_X]);
	gyro->filtered[GYRO_AXE_Y] =  FILTER_process(&gyro->filters[GYRO_AXE_Y], gyro->raw[GYRO_AXE_Y]);
	gyro->filtered[GYRO_AXE_Z] =  FILTER_process(&gyro->filters[GYRO_AXE_Z], gyro->raw[GYRO_AXE_Z]);
}

void GYRO_dma_done(gyro_t * gyro){
#ifdef GYRO_USE_MPU
	MPU_dma_transmit_done(gyro->mpu);
#else
#warning gyro not defined
#endif
}

