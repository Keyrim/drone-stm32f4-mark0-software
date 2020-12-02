/*
 * acc.c
 *
 *  Created on: 1 déc. 2020
 *      Author: Theo
 */


#include "acc.h"

sensor_state_e ACC_init(acc_t * acc, mpu_t * mpu){

	float filter_x_config [3] = {1, 0, 0};
	float filter_y_config [3] = {1, 0, 0};
	float filter_z_config [3] = {1, 0, 0};


	FILTER_init(&acc->filter[ACC_AXE_X], filter_x_config, FILTER_FIRST_ORDER);
	FILTER_init(&acc->filter[ACC_AXE_Y], filter_y_config, FILTER_FIRST_ORDER);
	FILTER_init(&acc->filter[ACC_AXE_Z], filter_z_config, FILTER_FIRST_ORDER);

#ifdef ACC_USE_MPU
	acc->mpu = mpu ;
	acc->raw = acc->mpu->acc ;
	acc->state = MPU_init_acc(acc->mpu, MPU_ACC_16G);
#else
#warning gyro not defined
	acc->state = SENSOR_ERROR ;
#endif
	return acc->state ;
}

sensor_state_e ACC_update(acc_t * acc){
#ifdef ACC_USE_MPU
	acc->state = MPU_update_acc(acc->mpu);
#else
#warning gyro not defined
	acc->state = SENSOR_ERROR ;
#endif

	return acc->state ;
}

void ACC_process_lpf(acc_t * acc){
	FILTER_process(&acc->filter[ACC_AXE_X], acc->raw[ACC_AXE_X]);
	FILTER_process(&acc->filter[ACC_AXE_Y], acc->raw[ACC_AXE_Y]);
	FILTER_process(&acc->filter[ACC_AXE_Z], acc->raw[ACC_AXE_Z]);
}






