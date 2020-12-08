/*
 * acc.c
 *
 *  Created on: 1 déc. 2020
 *      Author: Theo
 */


#include "acc.h"
#include "../OS/events/events.h"

static void acc_data_callback(void);

float filter_config [3] = {0.1f, 0.9f, 0};

void ACC_init(acc_t * acc, mpu_t * mpu){

	FILTER_init(&acc->filters[ACC_AXE_X], filter_config, FILTER_FIRST_ORDER);
	FILTER_init(&acc->filters[ACC_AXE_Y], filter_config, FILTER_FIRST_ORDER);
	FILTER_init(&acc->filters[ACC_AXE_Z], filter_config, FILTER_FIRST_ORDER);


	acc->mpu = mpu ;
	acc->raw = acc->mpu->acc ;

	switch(MPU_init_acc(acc->mpu, MPU_ACC_4G, acc_data_callback)){
		case SENSOR_REQUEST_OK:
			EVENT_Set_flag(FLAG_ACC_OK);
			break;
		case SENSOR_REQUEST_FAIL:
			break;
		case SENSOR_REQUEST_BUSY:
			break;
	}

}

void ACC_update(acc_t * acc){
	switch(MPU_update_acc(acc->mpu)){
		case SENSOR_REQUEST_OK:
			EVENT_Set_flag(FLAG_ACC_DATA_READY);
			break;
		case SENSOR_REQUEST_FAIL:
			EVENT_Clean_flag(FLAG_ACC_OK);
			break;
		case SENSOR_REQUEST_BUSY:
			break;
	}
}

void ACC_update_dma(acc_t * acc){
	switch(MPU_update_acc_dma(acc->mpu)){
		case SENSOR_REQUEST_OK:
			break;
		case SENSOR_REQUEST_FAIL:
			EVENT_Clean_flag(FLAG_ACC_OK);
			break;
		case SENSOR_REQUEST_BUSY:
			break;
	}
}

void ACC_process_lpf(acc_t * acc){
	acc->filtered[ACC_AXE_X] = FILTER_process(&acc->filters[ACC_AXE_X], acc->raw[ACC_AXE_X]);
	acc->filtered[ACC_AXE_Y] = FILTER_process(&acc->filters[ACC_AXE_Y], acc->raw[ACC_AXE_Y]);
	acc->filtered[ACC_AXE_Z] = FILTER_process(&acc->filters[ACC_AXE_Z], acc->raw[ACC_AXE_Z]);
	EVENT_Set_flag(FLAG_ACC_FILTERED_DATA_READY);
}

static void acc_data_callback(void){
	EVENT_Set_flag(FLAG_ACC_DATA_READY);
}






