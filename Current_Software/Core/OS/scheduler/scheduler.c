/*
 * scheduler.c
 *
 *  Created on: 12 ao�t 2020
 *      Author: Theo
 */
#include "scheduler.h"
#include "../time.h"

//Variables privées
static task_t * task_queu[TASK_COUNT+1];	//+1 sinon le memoove fait de la merde :/
static uint32_t task_queu_size = 0 ;
static uint32_t task_queu_position = 0 ;
static system_t * sys ;


static task_t * get_first_task(void);
static task_t * get_next_task(void);
static uint32_t task_process_it(task_t * task, uint32_t current_time_us);
static uint32_t task_process_main(task_t * task, uint32_t current_time_us);
static bool_e queu_contains(task_t * task);
static bool_e queu_add(task_t * task);
static bool_e queu_remove(task_t * task);
//static void queu_clear(void);		Not used



void SCHEDULER_init(system_t * sys_){
	//Mémorise l'emplacement données du système
	sys = sys_ ;

	//Init du debug pour le printf
	DEBUG_init();

	//Init des tâches
	tasks_init(sys_);
}

void SCHEDULER_run(void){
	uint32_t current_time_us = TIME_us();
	task_t * task = get_first_task();

	while(task_queu_position < task_queu_size && task != NULL){

		//TODO : Tâches par évennement
		if(task->static_priority != PRIORITY_REAL_TIME)
			if(current_time_us >= task->it_last_execution_us + task->it_desired_period_us)
					current_time_us = task_process_it(task, current_time_us);

		task = get_next_task();
	}
}

uint32_t SCHEDULER_get_cpu_load(void){
	uint32_t load_pourcentage = 0 ;
	for(uint32_t t = 0; t < task_queu_size; t++)
		load_pourcentage += task_queu[t]->it_duration_us * 100 / task_queu[t]->it_desired_period_us ;
	return load_pourcentage ;
}

static task_t * get_first_task(void){
	task_queu_position = 0 ;
	return task_queu[0] ;
}

static task_t * get_next_task(void){
	return task_queu[++task_queu_position];
}

static uint32_t task_process_it(task_t * task, uint32_t current_time_us){

	//Si la tâche n'as pas de fonction associée on s'arrête
	if(task->it == NULL)
		return current_time_us ;

	//Calcul de la période réelle en appliquant une moyenne glissante
	task->it_real_period_us_average_sum -= task->it_real_period_us_average_array[task->it_average_index];
	task->it_real_period_us_average_array[task->it_average_index] = current_time_us - task->it_last_execution_us ;
	task->it_real_period_us_average_sum += task->it_real_period_us_average_array[task->it_average_index] ;
	task->it_real_period_us = task->it_real_period_us_average_sum / TASK_STAT_AVERAGE_OVER ;

	task->it_last_execution_us = current_time_us ;
	task->it(current_time_us);
	current_time_us = TIME_us();


	//Calcul du "burst time" avec une moyenne glissante
	task->it_duration_us_average_sum -= task->it_duration_us_average_array[task->it_average_index];
	task->it_duration_us_average_array[task->it_average_index] = current_time_us - task->it_last_execution_us ;
	task->it_duration_us_worst = MAX(task->it_duration_us_average_array[task->it_average_index] , task->it_duration_us_worst);
	task->it_duration_us_average_sum += task->it_duration_us_average_array[task->it_average_index];
	task->it_duration_us = task->it_duration_us_average_sum / TASK_STAT_AVERAGE_OVER ;

	if(task->it_duration_us_average_array[task->it_average_index] > 1000)
		task->it_duration_us_average_array[task->it_average_index] ++ ;

	if(++task->it_average_index == TASK_STAT_AVERAGE_OVER)
		task->it_average_index = 0 ;
	return current_time_us ;
}

static uint32_t task_process_main(task_t * task, uint32_t current_time_us){

	//Si la tâche n'as pas de fonction associée on s'arrête
	if(task->main == NULL)
		return current_time_us ;

	//Calcul de la période en appliquant une moyenne glissante
	task->main_period_us_average_sum -= task->main_period_us_average_array[task->main_average_index];
	task->main_period_us_average_array[task->main_average_index] = current_time_us - task->main_last_execution_us ;
	task->main_period_us_average_sum += task->main_period_us_average_array[task->main_average_index] ;
	task->main_period_us = task->main_period_us_average_sum / TASK_STAT_AVERAGE_OVER ;

	task->main_last_execution_us = current_time_us ;
	task->main(current_time_us);
	current_time_us = TIME_us();


	//Calcul du "burst time" avec une moyenne glissante
	task->main_duration_us_average_sum -= task->main_duration_us_average_array[task->main_average_index];
	task->main_duration_us_average_array[task->main_average_index] = current_time_us - task->main_last_execution_us ;
	task->main_duration_us_worst = MAX(task->main_duration_us_average_array[task->main_average_index] , task->main_duration_us_worst);
	task->main_duration_us_average_sum += task->main_duration_us_average_array[task->main_average_index];
	task->main_duration_us = task->main_duration_us_average_sum / TASK_STAT_AVERAGE_OVER ;

	if(++task->main_average_index == TASK_STAT_AVERAGE_OVER)
		task->main_average_index = 0 ;
	return current_time_us ;
}


//Activation ou d�sactivation par ajout ou suppression dans la queu dans la queu
void SCHEDULER_enable_task(task_ids_t id, bool_e enable){
	if(enable && id < TASK_COUNT)
		queu_add(TASK_get_task(id));
	else
		queu_remove(TASK_get_task(id));
}

void SCHEDULER_reschedule_task(task_ids_t id, uint32_t new_period_us){
	TASK_get_task(id)->it_desired_period_us = new_period_us ;
}


/* to avoid not used warnings */
//static void queu_clear(void){
//	memset(task_queu, 0, sizeof(task_queu));
//	task_queu_size = 0 ;
//}

static bool_e queu_contains(task_t * task){
	for(uint32_t t = 0; t < task_queu_size; t++)
		if(task_queu[t] == task)
			return TRUE ;
	return FALSE ;
}

static bool_e queu_add(task_t * task){
	if(queu_contains(task) || task_queu_size >= TASK_COUNT)
		return FALSE ;


	uint32_t t = 0 ;
	while(t < TASK_COUNT)
	{
		if(task_queu[t] == NULL || task->static_priority > task_queu[t]->static_priority){
			if(task_queu[t] != NULL)
				memmove(&task_queu[t+1], &task_queu[t], sizeof(task) * (task_queu_size +1 - t));
			task_queu[t] = task ;
			task_queu_size ++ ;
			return TRUE ;
		}
		t++;
	}
	return FALSE ;
}

static bool_e queu_remove(task_t * task){
	if(!queu_contains(task))
		return FALSE ;

	for(uint32_t t = 0; t < task_queu_size; t ++){
		if(task == task_queu[t]){
			memmove(&task_queu[t], &task_queu[t+1], sizeof(task) * (task_queu_size - t));
			task_queu_size -- ;
			return TRUE ;
		}
	}
	return FALSE ;
}
