/*
 * gyro.c
 *
 *  Created on: Nov 22, 2020
 *      Author: Théo
 */


#include "gyro.h"


sensor_state_e GYRO_init(gyro_t * gyro){

#ifdef GYRO_USE_MPU
	gyro->raw = gyro->mpu.gyro ;
	gyro->state = MPU_init_gyro(&gyro->mpu, MPU_GYRO_2000s);
#else
#warning gyro not defined
	gyro->state = SENSOR_ERROR ;
#endif
	return gyro->state ;
}

sensor_state_e GYRO_update(gyro_t * gyro){

#ifdef GYRO_USE_MPU
	gyro->state = MPU_update_gyro(&gyro->mpu);
#else
#warning gyro not defined
	gyro->state = SENSOR_ERROR ;
#endif

	return gyro->state ;
}

sensor_state_e GYRO_update_dma(gyro_t * gyro){

#ifdef GYRO_USE_MPU
	gyro->state = MPU_update_gyro_dma(&gyro->mpu);
#else
#warning gyro not defined
	gyro->state = SENSOR_ERROR ;
#endif

	return gyro->state ;
}

void GYRO_dma_done(gyro_t * gyro){
#ifdef GYRO_USE_MPU
	MPU_dma_transmit_done(&gyro->mpu);
#else
#warning gyro not defined
#endif
}

