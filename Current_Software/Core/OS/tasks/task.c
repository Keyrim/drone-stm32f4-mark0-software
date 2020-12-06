/*
 * task.c
 *
 *  Created on: 13 aout 2020
 *      Author: Theo
 */

#include "task.h"
#include "../scheduler/scheduler.h"
#include "../events/events.h"

static system_t * sys;

void tasks_init(system_t * sys_){
	//On mémorise où se trouve notre structure système
	sys = sys_ ;

	//Activation des tâches
	SCHEDULER_enable_task(TASK_EVENT_CHECK, TRUE);

	SCHEDULER_enable_task(TASK_LED, TRUE);
	//SCHEDULER_enable_task(TASK_PRINTF, TRUE);


}

void process_print_f(uint32_t current_time_us){
	//printf("%d\t%d\t%d\t%d\t%d\t%d\n", data[0], data[1], data[2], data[3], data[4], data[5]);
	gyro_t * gyro = &sys->sensors.gyro ;
	acc_t * acc = &sys->sensors.acc ;
	//printf("%d\t%d\t%d\t%lu\n",sys->sensors.gyro.mpu->gyro_raw[0], sys->sensors.gyro.mpu->gyro_raw[1], sys->sensors.gyro.mpu->gyro_raw[2], TASK_get_task(TASK_GYRO)->it_duration_us);
	//printf("%f\t%f\t%f\t%f\t%f\t%f\t%lu\n",acc->raw[0], acc->raw[1], acc->raw[2],gyro->raw[0], gyro->raw[1], gyro->raw[2], TASK_get_task(TASK_GYRO_UPDATE)->duration_us);
	printf("%f\t%f\t%lu\n", acc->filtered[ACC_AXE_Z], gyro->filtered[ACC_AXE_X], SCHEDULER_get_cpu_load());
	//printf("%f\n", acc->filtered[GYRO_AXE_X]);
}


void process_led(uint32_t current_time_us){
	LED_SEQUENCE_play(&sys->ihm.led_blue, current_time_us);
	LED_SEQUENCE_play(&sys->ihm.led_red, current_time_us);
	LED_SEQUENCE_play(&sys->ihm.led_green, current_time_us);
}

void process_gyro_update(uint32_t current_time_us){
	GYRO_ACC_update_dma(&sys->sensors.gyro);

}

void process_gyro_filter(uint32_t current_time_us){
	GYRO_process_lpf(&sys->sensors.gyro);
}

void process_acc_update(uint32_t current_time_us){
	ACC_update(&sys->sensors.acc);
}

void process_acc_filter(uint32_t current_time_us){
	ACC_process_lpf(&sys->sensors.acc);
}

void process_event_main(uint32_t current_time_us){
	UNUSED(current_time_us);
	EVENT_process_events_main();
}

#define DEFINE_TASK(id_param, priority_param,  task_function_param, desired_period_us_param, mode_param) { 	\
	.id = id_param,										\
	.static_priority = priority_param,					\
	.process = task_function_param,						\
	.desired_period_us = desired_period_us_param,		\
	.mode = mode_param									\
}

#define PERIOD_US_FROM_HERTZ(hertz_param) (1000000 / hertz_param)

task_t tasks [TASK_COUNT] ={
		[TASK_EVENT_CHECK] = 	DEFINE_TASK(TASK_EVENT_CHECK, 		PRIORITY_EVENT, 		process_event_main, 		PERIOD_US_FROM_HERTZ(500), 	TASK_MODE_TIME),

		[TASK_PRINTF] = 		DEFINE_TASK(TASK_PRINTF, 			PRIORITY_LOW, 			process_print_f, 			PERIOD_US_FROM_HERTZ(60), 	TASK_MODE_TIME),
		[TASK_LED] = 			DEFINE_TASK(TASK_LED, 				PRIORITY_LOW,	 		process_led, 				PERIOD_US_FROM_HERTZ(500), 	TASK_MODE_TIME),

		[TASK_GYRO_UPDATE] = 	DEFINE_TASK(TASK_GYRO_UPDATE, 		PRIORITY_MEDIUM,	 	process_gyro_update, 		PERIOD_US_FROM_HERTZ(2500), 	TASK_MODE_TIME),
		[TASK_GYRO_FILTER] = 	DEFINE_TASK(TASK_GYRO_FILTER, 		PRIORITY_EVENT,	 		process_gyro_filter, 		PERIOD_US_FROM_HERTZ(1), 	TASK_MODE_EVENT),

		[TASK_ACC_UPDATE] = 	DEFINE_TASK(TASK_ACC_UPDATE, 		PRIORITY_MEDIUM,	 	process_acc_update, 		PERIOD_US_FROM_HERTZ(500), 	TASK_MODE_TIME),
		[TASK_ACC_FILTER] = 	DEFINE_TASK(TASK_ACC_FILTER, 		PRIORITY_EVENT,	 		process_acc_filter, 		PERIOD_US_FROM_HERTZ(1), 	TASK_MODE_EVENT),
};

task_t * TASK_get_task(task_ids_t id){
	return &tasks[id];
}


