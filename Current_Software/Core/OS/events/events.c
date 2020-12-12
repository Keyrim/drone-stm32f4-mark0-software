/*
 * events.c
 *
 *  Created on: 13 sept. 2020
 *      Author: Th�o
 */

#include "mask_def.h"
#include "flags.h"
#include "events.h"
#include "../scheduler/scheduler.h"

//Ensemble des flags
volatile static Mask_t flags ;
volatile static bool_e new_flag_it = FALSE ;
static TIM_HandleTypeDef * htim_event ;
static system_t * sys ;

//Si false, init du module event
static bool_e initialized = FALSE ;




#define DEFINE_EVENT(event_function_param, nb_mask_param, enable_param){  	\
		.function = event_function_param ,						\
		.nb_mask = nb_mask_param , 								\
		.state = enable_param									\
}


//	----------------------	Events main	--------------------------------------------------------------------------------
static void gyro_init_ok_func(mask_def_ids_t mask_id);
static void acc_init_ok_func(mask_def_ids_t mask_id);

//Attention !!!! nb_mask <= EVENT_NB_MASK_PER_EVENT_MAX else failure :)
static Event_t events_main[EVENT_MAIN_COUNT] ={
		//Events array
		[EVENT_MAIN_GYRO_INIT_OK] = DEFINE_EVENT(gyro_init_ok_func, 1, EVENT_ENABLED),
		[EVENT_MAIN_ACC_INIT_OK] = DEFINE_EVENT(acc_init_ok_func, 1, EVENT_DISABLED)
};

static void gyro_init_ok_func(mask_def_ids_t mask_id){
	//On dit que le gyro est ok et qu'il effectu des mesures
	EVENT_Set_flag(FLAG_GYRO_READING);
	//On lance la tâche d'update du gyro
	SCHEDULER_task_set_mode(TASK_GYRO_UPDATE, TASK_MODE_TIME);
	events_main[EVENT_MAIN_GYRO_INIT_OK].state = EVENT_DISABLED ;
}

static void acc_init_ok_func(mask_def_ids_t mask_id){
	//On dit que le gyro est ok et qu'il effectu des mesures
	EVENT_Set_flag(FLAG_ACC_READING);
	//On lance la tâche d'update du gyro
	SCHEDULER_task_set_mode(TASK_ACC_UPDATE, TASK_MODE_TIME);
	events_main[EVENT_MAIN_ACC_INIT_OK].state = EVENT_DISABLED ;
}

//	----------------------	Events it	----------------------------------------------------------------------------



static void gyro_data_ready_func(mask_def_ids_t mask_id);
static void acc_data_ready_func(mask_def_ids_t mask_id);
static void orientation_update(mask_def_ids_t mask_id);

//Définitions des events
//Attention !!!! nb_mask <= EVENT_NB_MASK_PER_EVENT_MAX sinon dérapage :)
static Event_t events_it[EVENT_IT_COUNT] ={
		//Events array
		[EVENT_IT_GYRO_DATA_READY] = 		DEFINE_EVENT(gyro_data_ready_func, 	1, EVENT_ENABLED),
		[EVENT_IT_ACC_DATA_READY] = 		DEFINE_EVENT(acc_data_ready_func, 	1, EVENT_ENABLED),
		[EVENT_IT_ORIENTATION_UPDATE] = 	DEFINE_EVENT(orientation_update, 	1, EVENT_ENABLED)
};

static void gyro_data_ready_func(mask_def_ids_t mask_id){
	__disable_irq();
	MASK_clean_flag(&flags, FLAG_GYRO_DATA_READY);
	__enable_irq();
	//On lance la tâche d'update du gyro
	SCHEDULER_task_set_mode(TASK_GYRO_FILTER, TASK_MODE_EVENT);
}

static void acc_data_ready_func(mask_def_ids_t mask_id){
	__disable_irq();
	MASK_clean_flag(&flags, FLAG_ACC_DATA_READY);
	__enable_irq();
	//On lance la tâche d'update du gyro
	SCHEDULER_task_set_mode(TASK_ACC_FILTER, TASK_MODE_EVENT);
}

static void orientation_update(mask_def_ids_t mask_id){
	__disable_irq();
	MASK_clean_flag(&flags, FLAG_ACC_FILTERED_DATA_READY);
	MASK_clean_flag(&flags, FLAG_GYRO_FILTERED_DATA_READY);
	__enable_irq();
	SCHEDULER_task_set_mode(TASK_ORIENTATION_UPDATE, TASK_MODE_EVENT);
}


//Déclenchement des events en main
void EVENT_process_events_main(){
	if(initialized){
	//Pout chaque event
		for(uint32_t e = 0; e < EVENT_MAIN_COUNT; e ++){

			uint32_t m = 0 ;
			bool_e function_did_run_once = FALSE ;
			//On test chaque paires de masque
			while(m < events_main[e].nb_mask && !function_did_run_once && events_main[e].state == EVENT_ENABLED){
				if(Mask_test_and(events_main[e].mask_and[m], flags)){		//Mask and test
					if(Mask_test_or(events_main[e].mask_or[m], flags)){		//Mask or test
						if(!Mask_test_or(events_main[e].mask_not[m], flags)){	//Mask not test
							events_main[e].function(m);
							function_did_run_once = TRUE ;
						}
					}
				}
				m++ ;
			}

		}
	}
}

//Déclenchement des events en it
void EVENT_process_events_it(){
	if(initialized){
	//Pout chaque event
		for(uint32_t e = 0; e < EVENT_IT_COUNT; e ++){
			uint32_t m = 0 ;
			bool_e function_did_run_once = FALSE ;
			//On test chaque paires de masque
			while(m < events_it[e].nb_mask && !function_did_run_once && events_it[e].state == EVENT_ENABLED){
				if(Mask_test_and(events_it[e].mask_and[m], flags)){		//Mask and test
					if(Mask_test_or(events_it[e].mask_or[m], flags)){		//Mask or test
						if(!Mask_test_or(events_it[e].mask_not[m], flags)){	//Mask not test
							events_it[e].function(m);
							function_did_run_once = TRUE ;
						}
					}
				}
				m++ ;

			}
		}
	}
}

//Set et clean depuis le main
bool_e EVENT_Set_flag(Flags_t flag){
	bool_e to_ret ;
	new_flag_it = TRUE ;
	__disable_irq();
	to_ret = MASK_set_flag(&flags, flag);		//It désactivitées pour éviter la réentrance
	__enable_irq();
	return to_ret ;
}
bool_e EVENT_Clean_flag(Flags_t flag){
	bool_e to_ret ;
	new_flag_it = TRUE ;
	__disable_irq();
	to_ret = MASK_clean_flag(&flags, flag);		//It désactivitées pour éviter la réentrance
	__enable_irq();
	return to_ret ;
}

//Set a flag, disable irq before use
bool_e EVENT_set_flag_it(Flags_t flag){
	new_flag_it = TRUE ;
	return MASK_set_flag(&flags, flag);
}
//Clear a flag, disable irq before use
bool_e EVENT_clean_flag_it(Flags_t flag){
	new_flag_it = TRUE ;
	return MASK_clean_flag(&flags, flag);
}

void EVENT_timmer_callback(TIM_HandleTypeDef * htim){
	if(new_flag_it){
		if(htim == htim_event){
			new_flag_it = FALSE ;
			EVENT_process_events_it();
		}
	}
}

void EVENT_init(system_t * sys_, TIM_HandleTypeDef * htim_event_){
	initialized = TRUE ;
	sys =sys_;

	//Configuration des mask associés aux events
	mask_def_events_it_init(events_it);
	mask_def_events_main_init(events_main);

	//On lance le timmer dédié à l'it event
	if(htim_event_ != NULL){
		htim_event = htim_event_ ;
		HAL_TIM_Base_Start_IT(htim_event);
	}
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  EVENT_timmer_callback(htim);
}




