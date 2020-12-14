/*
 * Regu_orientation.c
 *
 *  Created on: Dec 13, 2020
 *      Author: Théo
 */


#include "Regu_orientation.h"
#include "../Propulsion/Propulsion.h"
static regu_orientation_t * regu_orientation ;
static orientation_t * orientation ;

//	------------------------------	Angular Speed Regulation settings	-----------------------------------------
float filter_pid_velocity_roll_pitch[3] = {1.0f, 0.0f, 0.0f};
float filter_pid_velocity_yaw[3] = {1.0f, 0.0f, 0.0f};

float pid_velocity_settings_roll[PID_NB_SETTINGS] = {0.5f, 0.0f, 0.0f, REGU_ORIENTATION_FREQUENCY, 200};
float pid_velocity_settings_pitch[PID_NB_SETTINGS] = {0.5f, 0.0f, 0.0f, REGU_ORIENTATION_FREQUENCY, 200};
float pid_velocity_settings_yaw[PID_NB_SETTINGS] = {0.0f, 0.0f, 0.0f, REGU_ORIENTATION_FREQUENCY, 200};


//	------------------------------	Angular Pos Regulation settings	-----------------------------------------

void REGULATION_ORIENTATION_Init(regu_orientation_t * regu_orientation_, orientation_t * orientation_, int16_t * output){
	orientation = orientation_ ;
	regu_orientation = regu_orientation_ ;
	regu_orientation->mode = REGULATION_ORIENTATION_MODE_OFF ;
	regu_orientation->outputs = output ;
	//Init PIDs angular speed
	PID_init(&regu_orientation->pid_angular_speed[ORIENTATION_ROLL], pid_velocity_settings_roll, FILTER_FIRST_ORDER, filter_pid_velocity_roll_pitch);
	PID_init(&regu_orientation->pid_angular_speed[ORIENTATION_PITCH], pid_velocity_settings_pitch, FILTER_FIRST_ORDER, filter_pid_velocity_roll_pitch);
	PID_init(&regu_orientation->pid_angular_speed[ORIENTATION_YAW], pid_velocity_settings_yaw, FILTER_FIRST_ORDER, filter_pid_velocity_yaw);



}

void REGULATION_ORIENTATION_Set_Regulation_Mode(regulation_orientation_mode_e regu_mode){
	regu_orientation->mode = regu_mode;
}

void REGULATION_ORIENTATION_Process(void){

	switch(regu_orientation->mode){
		case REGULATION_ORIENTATION_MODE_OFF:
			//Set motors low
			break;
		case REGULATION_ORIENTATION_MODE_ACCRO:
			regu_orientation->outputs[PROP_CONSIGNE_ROLL] = (int16_t)PID_compute(&regu_orientation->pid_angular_speed[ORIENTATION_ROLL], regu_orientation->consigne_angular_speed[ORIENTATION_ROLL], orientation->angular_velocity[ORIENTATION_ROLL]);
			regu_orientation->outputs[PROP_CONSIGNE_PITCH] = (int16_t)PID_compute(&regu_orientation->pid_angular_speed[ORIENTATION_PITCH], regu_orientation->consigne_angular_speed[ORIENTATION_PITCH], orientation->angular_velocity[ORIENTATION_PITCH]);
			regu_orientation->outputs[PROP_CONSIGNE_YAW] = (int16_t)PID_compute(&regu_orientation->pid_angular_speed[ORIENTATION_YAW], regu_orientation->consigne_angular_speed[ORIENTATION_YAW], orientation->angular_velocity[ORIENTATION_YAW]);
			break;
		case REGULATION_ORIENTATION_MODE_ANGLE:
			break;
	}

}














