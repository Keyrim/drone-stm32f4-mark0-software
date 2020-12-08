/*
 * orientation.h
 *
 *  Created on: Dec 6, 2020
 *      Author: Théo
 */

#ifndef ESTIMATORS_ORIENTATION_H_
#define ESTIMATORS_ORIENTATION_H_

#include "../Sensors/gyro.h"
#include "../Sensors/acc.h"
#include "math.h"



typedef struct orientation_t{
	float angular_position [3];
	float * angular_velocity;
	float acc_angles[3];
	bool_e is_ok ;

	gyro_t * gyro ;
	acc_t * acc ;

	int32_t frequency ;
	float periode;

	float alpha ;

}orientation_t;

void ORIENTATION_Init(orientation_t * orientation, gyro_t * gyro, acc_t * acc, int32_t frequency);
void ORIENTATION_Update(orientation_t * orientation);



#endif /* ESTIMATORS_ORIENTATION_H_ */
