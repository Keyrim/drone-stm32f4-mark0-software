/*
 * Flight_mode.c
 *
 *  Created on: Dec 13, 2020
 *      Author: Théo
 */



#include "Flight_mode.h"

static system_t * sys ;
static flight_modes_names_e current_flight_mode = FLIGHT_MODE_ON_THE_GROUND;
static flight_modes_names_e next_flight_mode = FLIGHT_MODE_ON_THE_GROUND;


//	-------------------------	Prototypes	-------------------------
static void on_the_ground_entrance(void);
static void on_the_ground_main(void);

static void manual_accro_entrance(void);
static void manual_accro_main(void);
//	-------------------------	Define flights modes	-------------------------
#define DEF_FLIGHT_MODE(name_param, function_entrance_param, function_main_param) { 	\
	.name = name_param,																	\
	.function_main = function_main_param,												\
	.function_entrance = function_entrance_param										\
}

static flight_mode_t flights_modes[FLIGHT_MODE_COUNT] = {
		[FLIGHT_MODE_ON_THE_GROUND] = 	DEF_FLIGHT_MODE(FLIGHT_MODE_ON_THE_GROUND, 		on_the_ground_entrance, 	on_the_ground_main),
		[FLIGHT_MODE_MANUAL_ACCRO] = 	DEF_FLIGHT_MODE(FLIGHT_MODE_MANUAL_ACCRO, 		manual_accro_entrance, 		manual_accro_main)
};

//	-------------------------	Public functions	-------------------------
void FLIGHT_MODE_Init(system_t * sys_){
	sys = sys_;
}

//Dont call it from an it plz for now ..
void FLIGHT_MODE_Set_Flight_Mode(flight_modes_names_e new){
	next_flight_mode = new ;
}

void FLIGHT_MODE_Main(void){
	//Check if we have to change the current flight mode
	if(next_flight_mode != current_flight_mode){

		//Call the leave function associated to the current(previous) flight mode if it exists
		if(flights_modes[current_flight_mode].function_leave != NULL)
			flights_modes[current_flight_mode].function_leave();

		//Call the entrance function associated to the next flight mode if it exists
		if(flights_modes[next_flight_mode].function_entrance != NULL)
			flights_modes[next_flight_mode].function_entrance();

		//Change the current flight mode
		current_flight_mode = next_flight_mode ;
	}
	//Call the function associated to the current flight mode
	flights_modes[current_flight_mode].function_main();

}



//	-------------------------	Functions definitions	-------------------------
static void on_the_ground_entrance(void){
	REGULATION_ORIENTATION_Set_Regulation_Mode(REGULATION_ORIENTATION_MODE_OFF);
	REGULATION_POSITION_Set_Regulation_Mode(REGULATION_POSITION_MODE_OFF);
	IHM_Set_Sequences(LED_HIGH_LVL, SEQUENCE_LED_OFF, SEQUENCE_LED_1, SEQUENCE_LED_OFF);
}
static void on_the_ground_main(void){
}

static void manual_accro_entrance(void){
	REGULATION_ORIENTATION_Set_Regulation_Mode(REGULATION_ORIENTATION_MODE_ACCRO);
	REGULATION_POSITION_Set_Regulation_Mode(REGULATION_POSITION_MODE_ON);
	IHM_Set_Sequences(LED_HIGH_LVL, SEQUENCE_LED_OFF, SEQUENCE_LED_OFF, SEQUENCE_LED_1);
}

static void manual_accro_main(void){
	sys->regulation.orientation.consigne_angular_speed[ORIENTATION_ROLL] = -((float)sys->radio.controller.channels[0]-1500)*0.5f;
	sys->regulation.orientation.consigne_angular_speed[ORIENTATION_PITCH] = -((float)sys->radio.controller.channels[1]-1500)*0.5f;
	sys->regulation.orientation.consigne_angular_speed[ORIENTATION_YAW] = -((float)sys->radio.controller.channels[3]-1500)*0.5f;
	sys->regulation.position.consigne = sys->radio.controller.channels[2]-1000;
}
















