/*
 * acc.c
 *
 *  Created on: 1 déc. 2020
 *      Author: Theo
 */


#include "acc.h"

sensor_state_e ACC_init(acc_t * acc){
#ifdef ACC_USE_MPU
	acc->raw = acc->mpu.acc ;
	acc->state = MPU_init_acc(&acc->mpu, MPU_ACC_16G);
#else
#warning gyro not defined
	acc->state = SENSOR_ERROR ;
#endif
	return acc->state ;
}

sensor_state_e ACC_update(acc_t * acc){
#ifdef ACC_USE_MPU
	acc->state = MPU_update_acc(&acc->mpu);
#else
#warning gyro not defined
	acc->state = SENSOR_ERROR ;
#endif

	return acc->state ;
}
