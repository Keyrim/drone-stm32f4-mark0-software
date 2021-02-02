/*
 * position.c
 *
 *  Created on: 2 févr. 2021
 *      Author: Théo
 */

#include "position.h"

void POSITION_Init(position_t * position, orientation_t * orientation, acc_t * acc, int32_t frequency){

	//Links to recquired data structures
	position->orientation = orientation ;
	position->acc = acc ;
	position->acceleration_drone = acc->filtered ;

	//Configure our frequency
	position->frequency = frequency ;
	position->periode = (float)1 / (float)frequency ;

	//Init our values to zero
	for(int axe = 0; axe < 3; axe++)
	{
		position->position[axe] = 0 ;
		position->velocity[axe] = 0 ;
		position->acceleration[axe] = 0;
	}
}

void POSITION_Update(position_t * position){

	//Projette l'accélération du drone dans le "repère de la terre"
	float sin_alpha = sinf(position->orientation->angular_position[ORIENTATION_ROLL] * 0.01745329251f);
	float cos_alpha = cosf(position->orientation->angular_position[ORIENTATION_ROLL] * 0.01745329251f);
	float sin_beta = sinf(position->orientation->angular_position[ORIENTATION_PITCH] * 0.01745329251f);
	float cos_beta = cosf(position->orientation->angular_position[ORIENTATION_PITCH] * 0.01745329251f);

	float acc_x = position->acc->filtered[ORIENTATION_ROLL];
	float acc_y = position->acc->filtered[ORIENTATION_PITCH];
	float acc_z = position->acc->filtered[ORIENTATION_YAW];

	//Equations :
	// https://fr.wikipedia.org/wiki/Matrice_de_rotation
	// https://photos.google.com/photo/AF1QipPC2YkgS1dSJ5Y0lUlfNZ84SxoZOmHYG-Z7rT-n

	position->acceleration[POSITION_AXE_X] = acc_x * cos_beta + sin_beta * (sin_alpha * acc_y + cos_alpha * acc_z) ;
	position->acceleration[POSITION_AXE_Y] = acc_y * sin_beta - acc_z * sin_alpha ;
	position->acceleration[POSITION_AXE_Z] = cos_beta * (sin_alpha * acc_y + cos_alpha * acc_z) - acc_x * sin_beta ;




}
