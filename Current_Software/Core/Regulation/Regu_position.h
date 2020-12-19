/*
 * Regu_position.h
 *
 *  Created on: 15 déc. 2020
 *      Author: Théo
 */

#ifndef REGULATION_REGU_POSITION_H_
#define REGULATION_REGU_POSITION_H_


#include "stm32f4xx_hal.h"
#include "Regu_orientation.h"
#include "../Propulsion/Propulsion.h"


typedef enum regu_position_mode_e{
	REGULATION_POSITION_MODE_OFF,
	REGULATION_POSITION_MODE_ON
}regu_position_mode_e;

typedef struct reug_position_t{
	//TODO position_t pour la régu en position en suite
	int16_t consigne ;
	int16_t * outputs;
	regu_position_mode_e mode;

}regu_position_t;

void REGULATION_POSITION_Init(regu_position_t * regu_position_, regu_orientation_t * regu_orientation_, int16_t * outputs);
void REGULATION_POSITION_Set_Regulation_Mode(regu_position_mode_e regu_mode);
void REGULATION_POSITION_Process(void);

#endif /* REGULATION_REGU_POSITION_H_ */
