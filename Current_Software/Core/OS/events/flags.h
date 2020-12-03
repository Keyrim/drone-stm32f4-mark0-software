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
	FLAG_GYRO_INIT,
	FLAG_GYRO_OK,
	FLAG_GYRO_READING,
	FLAG_GYRO_DATA_READY,
	FLAG_GYRO_FILTERED_DATA_READY,
	FLAG_GYRO_DMA_DONE,
	FLAG_COUNT
}Flags_t;


#endif /* EVENTS_FLAGS_H_ */
