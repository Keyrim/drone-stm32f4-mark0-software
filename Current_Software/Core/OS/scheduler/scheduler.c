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

//Task to add or delete
static volatile task_t * task_to_moove[TASK_COUNT] ;
static volatile bool_e way_to_moove[TASK_COUNT] ;
static volatile uint8_t task_to_moove_counter;

static task_t * get_first_task(void);
static task_t * get_next_task(void);
static uint32_t task_process(task_t * task, uint32_t current_time_us);
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

		switch(task->mode){
			case TASK_MODE_ALWAYS :
				current_time_us = task_process(task, current_time_us);
				break;
			case TASK_MODE_TIME :
				if(current_time_us >= task->last_execution_us + task->desired_period_us)
					current_time_us = task_process(task, current_time_us);
				break;
			case TASK_MODE_EVENT :
				current_time_us = task_process(task, current_time_us);
				SCHEDULER_enable_task(task->id, FALSE);
				break;
			case TASK_MODE_TIMMER :
				//Task called from a timmer it
				break;
			default:
				break;
		}
		task = get_next_task();
	}
}

void SCHEDULER_task(void){
	//On reséquence les tâches en faisant attention aux tâches à supprimer ou rajouter
	for(uint8_t t = 0; t < task_to_moove_counter; t++)
	{
		if(way_to_moove[t])
			queu_add((task_t *)task_to_moove[t]);
		else
			queu_remove((task_t *)task_to_moove[t]);
	}
	task_to_moove_counter = 0 ;
}

uint32_t SCHEDULER_get_cpu_load(void){
	uint32_t load_pourcentage = 0 ;
	for(uint32_t t = 0; t < task_queu_size; t++)
		load_pourcentage += task_queu[t]->duration_us * 10000 / task_queu[t]->real_period_us ;
	return load_pourcentage ;
}

static task_t * get_first_task(void){
	task_queu_position = 0 ;
	return task_queu[0] ;
}

static task_t * get_next_task(void){
	return task_queu[++task_queu_position];
}

static uint32_t task_process(task_t * task, uint32_t current_time_us){

	//Si la tâche n'as pas de fonction associée on s'arrête
	if(task->process == NULL)
		return current_time_us ;

	//Calcul de la période réelle en appliquant une moyenne glissante
	task->real_period_us_average_sum -= task->real_period_us_average_array[task->average_index];
	task->real_period_us_average_array[task->average_index] = current_time_us - task->last_execution_us ;
	task->real_period_us_average_sum += task->real_period_us_average_array[task->average_index] ;
	task->real_period_us = task->real_period_us_average_sum / TASK_STAT_AVERAGE_OVER ;

	task->last_execution_us = current_time_us ;
	task->process(current_time_us);
	current_time_us = TIME_us();


	//Calcul du "burst time" avec une moyenne glissante
	task->duration_us_average_sum -= task->duration_us_average_array[task->average_index];
	task->duration_us_average_array[task->average_index] = current_time_us - task->last_execution_us ;
	task->duration_us_worst = MAX(task->duration_us_average_array[task->average_index] , task->duration_us_worst);
	task->duration_us_average_sum += task->duration_us_average_array[task->average_index];
	task->duration_us = task->duration_us_average_sum / TASK_STAT_AVERAGE_OVER ;

	if(task->duration_us_average_array[task->average_index] > 1000)
		task->duration_us_average_array[task->average_index] ++ ;

	if(++task->average_index == TASK_STAT_AVERAGE_OVER)
		task->average_index = 0 ;
	return current_time_us ;
}




//Activation ou d�sactivation par ajout ou suppression dans la queu dans la queu
void SCHEDULER_enable_task(task_ids_t id, bool_e enable){
	__disable_irq();
	task_to_moove[task_to_moove_counter] = TASK_get_task(id);
	way_to_moove[task_to_moove_counter++] = enable ;
	__enable_irq();
}

void SCHEDULER_reschedule_task(task_ids_t id, uint32_t new_period_us){
	TASK_get_task(id)->desired_period_us = new_period_us ;
}


/* to avoid not used warnings */
//static void queu_clear(void){
//	memset(task_queu, 0, sizeof(task_queu));
//	task_queu_size = 0 ;
//}

static bool_e queu_contains(task_t * task){
	if(task == NULL)
		return FALSE ;

	for(uint32_t t = 0; t < task_queu_size; t++)
		if(task_queu[t] == task)
			return TRUE ;
	return FALSE ;
}

static bool_e queu_add(task_t * task){

	if(queu_contains(task) || task_queu_size >= TASK_COUNT || task == NULL)
		return FALSE ;


	uint32_t t = 0 ;
	while(t < TASK_COUNT)
	{
		if(task_queu[t] == NULL)
		{
			task_queu[t] = task ;
			task_queu_size ++ ;
			return TRUE;
		}
		else if(task->static_priority > task_queu[t]->static_priority){
			memmove(&task_queu[t+1], &task_queu[t], sizeof(task) * (task_queu_size - t));
			task_queu[t] = task ;
			task_queu_size ++ ;
			return TRUE ;
		}
		t++;
	}
	return FALSE ;
}

static bool_e queu_remove(task_t * task){
	if(!queu_contains(task) || task == NULL)
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
