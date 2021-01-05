/*
 * orientation.c
 *
 *  Created on: Dec 6, 2020
 *      Author: Théo
 */


#include "orientation.h"

#define ORIENTATION_ALPHA 0.9998f

static bool_e first_use = TRUE ;

static float absolu(float x);
static float acc_correction(float x);
static float angle_180(float x);


static float angle_180(float x){
	if(x > 180)
		x -= 360;
	else if( x < -180)
		x += 360 ;
	return x ;
}

static float absolu(float x){
	if( x > 0)
		return x ;
	else
		return -x ;
}
static float acc_correction(float x){
	x = ( x > 0) ? 180 - x : -180 -x ;
	return x ;
}

void ORIENTATION_Init(orientation_t * orientation, gyro_t * gyro, acc_t * acc, int32_t frequency){
	//Link data structures
	orientation->acc = acc ;
	orientation->gyro = gyro ;
	orientation->angular_velocity = gyro->filtered ;

	//Configure our frequency
	orientation->frequency = frequency ;
	orientation->periode = (float)1 / (float)frequency ;

	orientation->alpha = ORIENTATION_ALPHA ;

	//Init our values to zero
	for(int x = 0; x < 3; x++)
	{
		orientation->angular_position[x] = 0 ;
		orientation->angular_velocity[x] = 0 ;
	}

}

void ORIENTATION_Update(orientation_t * orientation){
	//Just to make the code lighter
	acc_t * acc = orientation->acc ;
	gyro_t * gyro = orientation->gyro ;

	//Guess our angular position using the accelerometer

	//Total acceleration
	float acc_total = acc->filtered[ACC_AXE_X] * acc->filtered[ACC_AXE_X] ;
	acc_total += acc->filtered[ACC_AXE_Y] * acc->filtered[ACC_AXE_Y] ;
	acc_total += acc->filtered[ACC_AXE_Z] * acc->filtered[ACC_AXE_Z] ;
	acc_total = sqrtf(acc_total);

	if(acc_total)	//To avoid /0
	{
		if(absolu(acc->filtered[ACC_AXE_X]) < acc_total)	//To avoid asin x with x greater than 1
			orientation->acc_angles[ACC_AXE_Y] = -asinf(acc->filtered[ACC_AXE_X] / acc_total) * 57.32f;
		if(absolu(acc->filtered[ACC_AXE_Y]) < acc_total) 	//To avoid asin x with x greater than 1
			orientation->acc_angles[ACC_AXE_X] = asinf(acc->filtered[ACC_AXE_Y] / acc_total) * 57.32f;
		if(acc->filtered[ACC_AXE_Z] < 0)
		{
			orientation->acc_angles[ACC_AXE_X] = acc_correction(orientation->acc_angles[ACC_AXE_X]);
			orientation->acc_angles[ACC_AXE_Y] = acc_correction(orientation->acc_angles[ACC_AXE_Y]);
		}
	}

	//If it's our first use, we dont use the gyro
	if(first_use)
	{
		orientation->angular_position[GYRO_AXE_X] = orientation->acc_angles[ACC_AXE_X] ;
		orientation->angular_position[GYRO_AXE_Y] = orientation->acc_angles[ACC_AXE_Y] ;
		orientation->angular_position[GYRO_AXE_Z] = 0 ;
		first_use = FALSE ;
	}
	else
	{
		//Angular distance according to our angular velocities
		float dx = gyro->filtered[GYRO_AXE_X] * orientation->periode ;
		float dy = gyro->filtered[GYRO_AXE_Y] * orientation->periode ;
		float dz = gyro->filtered[GYRO_AXE_Z] * orientation->periode ;

		orientation->angular_position[GYRO_AXE_X] += dx ;
		orientation->angular_position[GYRO_AXE_Y] += dy ;
		orientation->angular_position[GYRO_AXE_Z] += dz ;


		orientation->angular_position[GYRO_AXE_X] = angle_180(orientation->angular_position[GYRO_AXE_X]);
		orientation->angular_position[GYRO_AXE_Y] = angle_180(orientation->angular_position[GYRO_AXE_Y]);

		orientation->angular_position[GYRO_AXE_X] = orientation->alpha * orientation->angular_position[GYRO_AXE_X] + (1.0f - orientation->alpha) * orientation->acc_angles[ACC_AXE_X];
		orientation->angular_position[GYRO_AXE_Y] = orientation->alpha * orientation->angular_position[GYRO_AXE_Y] + (1.0f - orientation->alpha) * orientation->acc_angles[ACC_AXE_Y];



	}
}






















