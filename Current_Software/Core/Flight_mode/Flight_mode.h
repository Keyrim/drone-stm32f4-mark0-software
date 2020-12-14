/*
 * Flight_mode.h
 *
 *  Created on: Dec 13, 2020
 *      Author: Théo
 */

#ifndef FLIGHT_MODE_FLIGHT_MODE_H_
#define FLIGHT_MODE_FLIGHT_MODE_H_

#include "../OS/system_d.h"

typedef enum flight_modes_names_e{
	FLIGHT_MODE_ON_THE_GROUND = 0x00,
	FLIGHT_MODE_MANUAL_ACCRO,
	FLIGHT_MODE_COUNT
}flight_modes_names_e;

typedef struct flight_mode_t{
	flight_modes_names_e name;
	void (*function_entrance)(void);
	void (*function_main)(void);
	void (*function_leave)(void);
}flight_mode_t;

void FLIGHT_MODE_Init(system_t * sys_);
void FLIGHT_MODE_Set_Flight_Mode(flight_modes_names_e new);
void FLIGHT_MODE_Main(void);

#endif /* FLIGHT_MODE_FLIGHT_MODE_H_ */
