/*
 * Regu_position.c
 *
 *  Created on: 15 déc. 2020
 *      Author: Théo
 */


#include "Regu_position.h"

static regu_position_t * regu_position ;
static regu_orientation_t * regu_orientation ;





void REGULATION_POSITION_Init(regu_position_t * regu_position_, regu_orientation_t * regu_orientation_, int16_t * outputs){
	regu_position = regu_position_;
	regu_orientation = regu_orientation_;
	regu_position->mode = REGULATION_POSITION_MODE_OFF ;
	regu_position->outputs = outputs ;


}

void REGULATION_POSITION_Set_Regulation_Mode(regu_position_mode_e regu_mode){
	regu_position->mode = regu_mode ;
}

void REGULATION_POSITION_Process(void){
	switch(regu_position->mode){
		case REGULATION_POSITION_MODE_OFF:
			regu_position->outputs[PROP_CONSIGNE_THRUST] = 0;
			break;
		case REGULATION_POSITION_MODE_ON:
			regu_position->outputs[PROP_CONSIGNE_THRUST] = regu_position->consigne;
			break;
	}
}






























