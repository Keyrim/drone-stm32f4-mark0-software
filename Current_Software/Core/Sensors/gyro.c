/*
 * gyro.c
 *
 *  Created on: Nov 22, 2020
 *      Author: Théo
 */


#include "gyro.h"
#include "../OS/events/events.h"

float filter[3] = {0.1f, 0.9f, 0};

static void gyro_data_callback(void);

void GYRO_init(gyro_t * gyro, mpu_t * mpu){

	//Filters init
	FILTER_init(&gyro->filters[GYRO_AXE_X], filter, FILTER_FIRST_ORDER);
	FILTER_init(&gyro->filters[GYRO_AXE_Y], filter, FILTER_FIRST_ORDER);
	FILTER_init(&gyro->filters[GYRO_AXE_Z], filter, FILTER_FIRST_ORDER);

	gyro->mpu = mpu ;
	gyro->raw = gyro->mpu->gyro ;

	//Raise the flag "ok" if the gyro get succesfully initiated
	switch(MPU_init_gyro(gyro->mpu, MPU_GYRO_2000s, gyro_data_callback)){
		case SENSOR_REQUEST_OK:
			EVENT_Set_flag(FLAG_GYRO_OK);
			break;
		case SENSOR_REQUEST_FAIL:
			break;
		case SENSOR_REQUEST_BUSY:
			break;
	}
}

void GYRO_update(gyro_t * gyro){
	switch(MPU_update_gyro(gyro->mpu)){
		case SENSOR_REQUEST_OK:
			EVENT_Set_flag(FLAG_GYRO_DATA_READY);
			break;
		case SENSOR_REQUEST_FAIL:
			EVENT_Clean_flag(FLAG_GYRO_OK);
			break;
		case SENSOR_REQUEST_BUSY:
			break;
	}
}

void GYRO_ACC_update(gyro_t * gyro){
	switch(MPU_update_all(gyro->mpu)){
		case SENSOR_REQUEST_OK:
			EVENT_Set_flag(FLAG_GYRO_DATA_READY);
			EVENT_Set_flag(FLAG_ACC_DATA_READY);
			break;
		case SENSOR_REQUEST_FAIL:
			EVENT_Clean_flag(FLAG_GYRO_OK);
			break;
		case SENSOR_REQUEST_BUSY:
			break;
	}
}

void GYRO_update_dma(gyro_t * gyro){
	switch(MPU_update_gyro_dma(gyro->mpu)){
		case SENSOR_REQUEST_OK:
			break;
		case SENSOR_REQUEST_FAIL:
			EVENT_Clean_flag(FLAG_GYRO_OK);
			EVENT_Clean_flag(FLAG_ACC_OK);
			break;
		case SENSOR_REQUEST_BUSY:
			break;
	}
}

void GYRO_ACC_update_dma(gyro_t * gyro){
	switch(MPU_update_all_dma(gyro->mpu)){
		case SENSOR_REQUEST_OK:
			break;
		case SENSOR_REQUEST_FAIL:
			EVENT_Clean_flag(FLAG_GYRO_OK);
			EVENT_Clean_flag(FLAG_ACC_OK);
			break;
		case SENSOR_REQUEST_BUSY:
			break;
	}
}

void GYRO_process_lpf(gyro_t * gyro){
	gyro->filtered[GYRO_AXE_X] =  FILTER_process(&gyro->filters[GYRO_AXE_X], gyro->raw[GYRO_AXE_X]);
	gyro->filtered[GYRO_AXE_Y] =  FILTER_process(&gyro->filters[GYRO_AXE_Y], gyro->raw[GYRO_AXE_Y]);
	gyro->filtered[GYRO_AXE_Z] =  FILTER_process(&gyro->filters[GYRO_AXE_Z], gyro->raw[GYRO_AXE_Z]);
	EVENT_Set_flag(FLAG_GYRO_FILTERED_DATA_READY);
}

static void gyro_data_callback(void){
	EVENT_Set_flag(FLAG_GYRO_DATA_READY);
}






