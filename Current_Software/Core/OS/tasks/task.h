/*
 * task.h
 *
 *  Created on: 13 août 2020
 *      Author: Théo
 */

#ifndef TASKS_TASK_H_
#define TASKS_TASK_H_
#include "stdint.h"
#include "stdio.h"
#include "../system_d.h"

#define TASK_STAT_AVERAGE_OVER 20		//Moyenne glissante sur X valeurs
#define TASK_DURATION_MAX	800	//On considère  qu une tache ne peut durer plus de 800µS sinon il y a un GROS problème


typedef enum task_priority{
	PRIORITY_REAL_TIME 	= -1 ,
	//Priorité toujours positives
	PRIORITY_LOW		= 1 ,
	PRIORITY_MEDIUM	 	= 5 ,
	PRIORITY_HIGH		= 10
}task_priority_t;

typedef enum task_states{
	INIT,				//	Tâche
	READY				// 	Tâches plac�es dans la file d attente pour �tre trait�
}task_states_t;

typedef enum task_ids {
	TASK_PRINTF,
	TASK_LED,
	TASK_GYRO,
	TASK_COUNT
}task_ids_t;

typedef enum{
	PROCESS_IT,
	PROCESS_MAIN
}process_type_e;

typedef struct task{
	//Propriétées de la tâche
	task_priority_t static_priority ;
	task_ids_t id ;

	//Process
	void (*main)(uint32_t current_time_us);
	void (*it)(uint32_t current_time_us);

	//stat
	uint32_t it_duration_us ;
	uint32_t it_duration_us_worst ;
	uint32_t it_desired_period_us ;
	uint32_t it_real_period_us ;
	uint32_t it_last_execution_us ;

	uint32_t main_duration_us ;
	uint32_t main_duration_us_worst ;
	uint32_t main_period_us ;
	uint32_t main_last_execution_us ;

	//Tableau pour avoir les stats en moyenne glissante
	uint32_t it_real_period_us_average_array [TASK_STAT_AVERAGE_OVER];
	uint32_t it_real_period_us_average_sum ;
	uint32_t it_duration_us_average_array [TASK_STAT_AVERAGE_OVER];
	uint32_t it_duration_us_average_sum ;
	uint32_t it_average_index ;

	uint32_t main_period_us_average_array [TASK_STAT_AVERAGE_OVER];
	uint32_t main_period_us_average_sum ;
	uint32_t main_duration_us_average_array [TASK_STAT_AVERAGE_OVER];
	uint32_t main_duration_us_average_sum ;
	uint32_t main_average_index ;
}task_t;

void tasks_init(system_t * sys_);
task_t * TASK_get_task(task_ids_t id);

#endif /* TASKS_TASK_H_ */
