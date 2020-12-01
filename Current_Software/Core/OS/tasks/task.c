/*
 * task.c
 *
 *  Created on: 13 aout 2020
 *      Author: Theo
 */

#include "task.h"
#include "../scheduler/scheduler.h"

static system_t * sys;

void tasks_init(system_t * sys_){
	//On mémorise où se trouve notre structure système
	sys = sys_ ;

	//Activation des tâches
	SCHEDULER_enable_task(TASK_LED, TRUE);
	SCHEDULER_enable_task(TASK_PRINTF, TRUE);
	SCHEDULER_enable_task(TASK_GYRO, TRUE);

}

void task_main_print_f(uint32_t current_time_us){
	uint8_t * data = sys->sensors.gyro.mpu->gyro_data;
	//printf("%d\t%d\t%d\t%d\t%d\t%d\n", data[0], data[1], data[2], data[3], data[4], data[5]);

	//printf("%d\t%d\t%d\t%lu\n",sys->sensors.gyro.mpu->gyro_raw[0], sys->sensors.gyro.mpu->gyro_raw[1], sys->sensors.gyro.mpu->gyro_raw[2], TASK_get_task(TASK_GYRO)->it_duration_us);
	printf("%f\t%f\t%f\t%lu\n",sys->sensors.gyro.raw[0], sys->sensors.gyro.raw[1], sys->sensors.gyro.raw[2], TASK_get_task(TASK_GYRO)->it_duration_us);
	//printf("%d\n", sys->sensors.gyro.mpu->gyro_raw[0]);
}


void task_main_led(uint32_t current_time_us){
	LED_SEQUENCE_play(&sys->ihm.led_blue, current_time_us);
	LED_SEQUENCE_play(&sys->ihm.led_red, current_time_us);
	LED_SEQUENCE_play(&sys->ihm.led_green, current_time_us);
}

void task_it_gyro(uint32_t current_time_us){
	GYRO_update(&sys->sensors.gyro);
}

#define DEFINE_TASK(id_param, priority_param,  task_function_param, desired_period_us_param) { 	\
	.id = id_param,										\
	.static_priority = priority_param,					\
	.it = task_function_param,					\
	.it_desired_period_us = desired_period_us_param		\
}

#define PERIOD_US_FROM_HERTZ(hertz_param) (1000000 / hertz_param)

task_t tasks [TASK_COUNT] ={
		[TASK_PRINTF] = 		DEFINE_TASK(TASK_PRINTF, 			PRIORITY_HIGH, 			task_main_print_f, 			PERIOD_US_FROM_HERTZ(20)),
		[TASK_LED] = 			DEFINE_TASK(TASK_LED, 				PRIORITY_LOW,	 		task_main_led, 				PERIOD_US_FROM_HERTZ(10)),
		[TASK_GYRO] = 			DEFINE_TASK(TASK_GYRO, 				PRIORITY_HIGH,	 		task_it_gyro, 				PERIOD_US_FROM_HERTZ(20))
};

task_t * TASK_get_task(task_ids_t id){
	return &tasks[id];
}


