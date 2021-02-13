/*
 * position.c
 *
 *  Created on: 2 févr. 2021
 *      Author: Théo
 */

#include "position.h"
#include "../OS/events/events.h"

#define ALPHA 0.08f
#define BETA 0.001f
#define GAMMA 0.5f

void POSITION_Init(position_t * position, orientation_t * orientation, acc_t * acc, baro_t * baro, int32_t frequency){

	//Links to recquired data structures
	position->orientation = orientation ;
	position->acc = acc ;
	position->baro = baro;
	position->acceleration_drone = acc->filtered ;

	//Configure our frequency
	position->frequency = frequency ;
	position->periode = (float)1 / (float)frequency ;
	position->periode_squared = position->periode * position->periode ;



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
	position->acceleration[POSITION_AXE_Z] = ( cos_beta * (sin_alpha * acc_y + cos_alpha * acc_z) - acc_x * sin_beta ) - 1 ; //Minus to compensate the gravity

	//From acceleration in g to m/s²
	position->acceleration[POSITION_AXE_X] *= 9.81 ;
	position->acceleration[POSITION_AXE_Y] *= 9.81 ;
	position->acceleration[POSITION_AXE_Z] *= 9.81 ;

	//If life was simple :
//	position->velocity[POSITION_AXE_X] += position->acceleration[POSITION_AXE_X] * position->periode ;
//	position->velocity[POSITION_AXE_Y] += position->acceleration[POSITION_AXE_Y] * position->periode ;
//	position->velocity[POSITION_AXE_Z] += position->acceleration[POSITION_AXE_Z] * position->periode ;
//
//	position->position[POSITION_AXE_X] += position->velocity[POSITION_AXE_X] * position->periode ;
//	position->position[POSITION_AXE_Y] += position->velocity[POSITION_AXE_Y] * position->periode ;
//	position->position[POSITION_AXE_Z] += position->velocity[POSITION_AXE_Z] * position->periode ;


	//Alpha beta gamma filter to estimate the altitude

	//We need our first measurement to init the filter
	if(!position->alti_is_init)
	{
		if(EVENT_Check_flag(FLAG_BARO_ALTITUDE_RDY)){
			position->position[POSITION_AXE_Z] = *position->baro->altitude;
			position->alti_is_init = TRUE ;
			EVENT_Clean_flag(FLAG_BARO_ALTITUDE_RDY);
		}
	}
	//Filter is init
	else
	{


		//								----------------------- State update equation ---------------------
		if(EVENT_Check_flag(FLAG_BARO_ALTITUDE_RDY)){
			float measurement = *position->baro->altitude;
			EVENT_Clean_flag(FLAG_BARO_ALTITUDE_RDY);
			position->position[POSITION_AXE_Z] = position->position_guess[POSITION_AXE_Z] + ALPHA * (measurement - position->position_guess[POSITION_AXE_Z]);
			position->velocity[POSITION_AXE_Z] = position->velocity_guess[POSITION_AXE_Z] + BETA * ((measurement - position->position_guess[POSITION_AXE_Z])/0.01f);
		}
		else
		{
			position->position[POSITION_AXE_Z] = position->position_guess[POSITION_AXE_Z] ;
			position->velocity[POSITION_AXE_Z] = position->velocity_guess[POSITION_AXE_Z] ;
		}
		position->acceleration[POSITION_AXE_Z] = position->acceleration_guess[POSITION_AXE_Z] + GAMMA * (position->acceleration[POSITION_AXE_Z] - position->acceleration_guess[POSITION_AXE_Z]);

		//								----------------------- State extrapolation equation ---------------------
		position->position_guess[POSITION_AXE_Z] = position->position[POSITION_AXE_Z] + (position->velocity[POSITION_AXE_Z]  + position->acceleration[POSITION_AXE_Z] * 0.5f * position->periode) * position->periode ;
		position->velocity_guess[POSITION_AXE_Z] = position->velocity[POSITION_AXE_Z] + position->acceleration[POSITION_AXE_Z] * position->periode ;
		position->acceleration_guess[POSITION_AXE_Z] = position->acceleration[POSITION_AXE_Z];

	}






}









