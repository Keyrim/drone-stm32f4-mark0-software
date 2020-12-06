/*
 * flags.h
 *
 *  Created on: 16 sept. 2020
 *      Author: Theo
 */

#ifndef EVENTS_FLAGS_H_
#define EVENTS_FLAGS_H_

typedef enum{
	//Flags

	//GYRO
	FLAG_GYRO_OK,
	FLAG_GYRO_READING,
	FLAG_GYRO_DATA_READY,
	FLAG_GYRO_FILTERED_DATA_READY,

	//ACC
	FLAG_ACC_OK,
	FLAG_ACC_READING,
	FLAG_ACC_DATA_READY,
	FLAG_ACC_FILTERED_DATA_READY,

	FLAG_COUNT
}Flags_t;


#endif /* EVENTS_FLAGS_H_ */
