/*
 * mask_def.c
 *
 *  Created on: 17 sept. 2020
 *      Author: Theo
 */

#include "mask_def.h"



//Prototypes
// ----------------	GYRO ------------------------
static void mask_def_gyro_data_ready(Event_t * event);
static void mask_def_gyro_init_ok(Event_t * event);

// ----------------	ACC ------------------------
static void mask_def_acc_data_ready(Event_t * event);
static void mask_def_acc_init_ok(Event_t * event);

// ----------------	Orientation ------------------------
static void mask_def_orientation_update(Event_t * event);

// ----------------	Radio ------------------------
static void mask_def_ibus_data_rdy(Event_t * event);

// ----------------	Flight Modes ------------------------
static void mask_def_on_the_ground(Event_t * event);
static void mask_def_manual_accro(Event_t * event);
//----------------	GLOBAL INIT ------------------------
void mask_def_events_it_init(Event_t * event){
	//Gyro mask def
	mask_def_gyro_data_ready(&event[EVENT_IT_GYRO_DATA_READY]);

	//Acc mask def
	mask_def_acc_data_ready(&event[EVENT_IT_ACC_DATA_READY]);

	//Orientation
	mask_def_orientation_update(&event[EVENT_IT_ORIENTATION_UPDATE]);



}

void mask_def_events_main_init(Event_t * event){
	//Gyro mask def
	mask_def_gyro_init_ok(&event[EVENT_MAIN_GYRO_INIT_OK]);

	//Acc mask def
	mask_def_acc_init_ok(&event[EVENT_MAIN_ACC_INIT_OK]);

	//Ibus
	mask_def_ibus_data_rdy(&event[EVENT_MAIN_IBUS_DATA_RDY]);

	//Flight_modes
	mask_def_on_the_ground(&event[EVENT_MAIN_ON_THE_GROUND]);
	mask_def_manual_accro(&event[EVENT_MAIN_MANUAL_ACCRO]);

}

// ----------------	GYRO ------------------------
static void mask_def_gyro_init_ok(Event_t * event){
	//Si le gyro est init et pas déjà en train de tourner
	MASK_set_flag(&event->mask_and[MASK_GYRO_INIT_OK], FLAG_GYRO_OK);
	MASK_set_flag(&event->mask_or[MASK_GYRO_INIT_OK], FLAG_GYRO_OK);
	MASK_set_flag(&event->mask_not[MASK_GYRO_INIT_OK], FLAG_GYRO_READING);
}

static void mask_def_gyro_data_ready(Event_t * event){
	//Si le gyro est init et pas déjà en train de tourner
	MASK_set_flag(&event->mask_and[MASK_GYRO_DATA_READY], FLAG_GYRO_OK);
	MASK_set_flag(&event->mask_or[MASK_GYRO_DATA_READY], FLAG_GYRO_DATA_READY);
}

// ----------------	ACC ------------------------

static void mask_def_acc_init_ok(Event_t * event){
	//Si le gyro est init et pas déjà en train de tourner
	MASK_set_flag(&event->mask_and[MASK_ACC_INIT_OK], FLAG_ACC_OK);
	MASK_set_flag(&event->mask_or[MASK_ACC_INIT_OK], FLAG_ACC_OK);
	MASK_set_flag(&event->mask_not[MASK_ACC_INIT_OK], FLAG_ACC_READING);
}

static void mask_def_acc_data_ready(Event_t * event){
	//Si le gyro est init et pas déjà en train de tourner
	MASK_set_flag(&event->mask_and[MASK_ACC_DATA_READY], FLAG_ACC_OK);
	MASK_set_flag(&event->mask_or[MASK_ACC_DATA_READY], FLAG_ACC_DATA_READY);
}

// ----------------	ORIENTATION ------------------------
static void mask_def_orientation_update(Event_t * event){
	//Si les données filtrées du gyro et de l'acc on go
	MASK_set_flag(&event->mask_and[MASK_ORIENTATION_UPDATE], FLAG_ACC_OK);
	MASK_set_flag(&event->mask_and[MASK_ORIENTATION_UPDATE], FLAG_GYRO_OK);
	MASK_set_flag(&event->mask_and[MASK_ORIENTATION_UPDATE], FLAG_GYRO_FILTERED_DATA_READY);
	MASK_set_flag(&event->mask_or[MASK_ORIENTATION_UPDATE], FLAG_ACC_FILTERED_DATA_READY);
}

// ----------------	Radio ------------------------

static void mask_def_ibus_data_rdy(Event_t * event){
	//Si flag data rdy on y go
	MASK_set_flag(&event->mask_and[MASK_IBUS_DATA_RDY], FLAG_IBUS_DATA_RDY);
	MASK_set_flag(&event->mask_or[MASK_IBUS_DATA_RDY], FLAG_IBUS_DATA_RDY);
}

// ----------------	Flight Modes ------------------------
static void mask_def_on_the_ground(Event_t * event){
	MASK_set_flag(&event->mask_and[MASK_ON_THE_GROUND_MANUAL], FLAG_FLIGHT_MODE_MANUAL_ACCRO);

	MASK_set_flag(&event->mask_or[MASK_ON_THE_GROUND_MANUAL], FLAG_THROTTLE_NULL);
	MASK_set_flag(&event->mask_or[MASK_ON_THE_GROUND_MANUAL], FLAG_CHAN_5_POS_1);

}

static void mask_def_manual_accro(Event_t * event){
	MASK_set_flag(&event->mask_and[MASK_MANUAL_ON_THE_GROUND], FLAG_FLIGHT_MODE_ON_THE_GROUND);
	MASK_set_flag(&event->mask_and[MASK_MANUAL_ON_THE_GROUND], FLAG_CHAN_5_POS_3);

	MASK_set_flag(&event->mask_or[MASK_MANUAL_ON_THE_GROUND], FLAG_THROTTLE_LOW);

}




