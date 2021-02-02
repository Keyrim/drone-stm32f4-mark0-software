/*
 * position.h
 *
 *  Created on: 2 févr. 2021
 *      Author: Théo
 */

#ifndef ESTIMATORS_POSITION_H_
#define ESTIMATORS_POSITION_H_

#include "orientation.h"

typedef enum position_axes_e{
	POSITION_AXE_X = 0x00,
	POSITION_AXE_Y,
	POSITION_AXE_Z,
	POSITION_AXES_COUNT
}position_axes_e;

typedef struct position_t{
	float position [POSITION_AXES_COUNT];
	float velocity [POSITION_AXES_COUNT];
	float acceleration [POSITION_AXES_COUNT];
	float * acceleration_drone;

	acc_t * acc ;
	orientation_t * orientation ;

	int32_t frequency ;
	float periode ;

}position_t;

void POSITION_Init(position_t * position, orientation_t * orientation, acc_t * acc, int32_t frequency);
void POSITION_Update(position_t * position);
#endif /* ESTIMATORS_POSITION_H_ */
