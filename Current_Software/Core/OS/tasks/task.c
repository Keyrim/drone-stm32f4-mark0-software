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
	SCHEDULER_enable_task(TASK_GYRO_UPDATE, TRUE);
	SCHEDULER_enable_task(TASK_ACC_UPDATE, TRUE);

}

void process_print_f(uint32_t current_time_us){
	//printf("%d\t%d\t%d\t%d\t%d\t%d\n", data[0], data[1], data[2], data[3], data[4], data[5]);
	//gyro_t * gyro = &sys->sensors.gyro ;
	acc_t * acc = &sys->sensors.acc ;
	//printf("%d\t%d\t%d\t%lu\n",sys->sensors.gyro.mpu->gyro_raw[0], sys->sensors.gyro.mpu->gyro_raw[1], sys->sensors.gyro.mpu->gyro_raw[2], TASK_get_task(TASK_GYRO)->it_duration_us);
	//printf("%f\t%f\t%f\t%f\t%f\t%f\t%lu\n",acc->raw[0], acc->raw[1], acc->raw[2],gyro->raw[0], gyro->raw[1], gyro->raw[2], TASK_get_task(TASK_GYRO_UPDATE)->duration_us);
	printf("%f\t%f\n", acc->raw[ACC_AXE_Z], acc->filtered[ACC_AXE_Z]);
	//printf("%d\n", sys->sensors.gyro.mpu->gyro_raw[0]);
}


void process_led(uint32_t current_time_us){
	LED_SEQUENCE_play(&sys->ihm.led_blue, current_time_us);
	LED_SEQUENCE_play(&sys->ihm.led_red, current_time_us);
	LED_SEQUENCE_play(&sys->ihm.led_green, current_time_us);
}

void process_gyro_update(uint32_t current_time_us){
	GYRO_update(&sys->sensors.gyro);
	GYRO_process_lpf(&sys->sensors.gyro);
}

void process_acc_update(uint32_t current_time_us){
	ACC_update(&sys->sensors.acc);
	ACC_process_lpf(&sys->sensors.acc);
}

#define DEFINE_TASK(id_param, priority_param,  task_function_param, desired_period_us_param) { 	\
	.id = id_param,										\
	.static_priority = priority_param,					\
	.process = task_function_param,						\
	.desired_period_us = desired_period_us_param		\
}

#define PERIOD_US_FROM_HERTZ(hertz_param) (1000000 / hertz_param)

task_t tasks [TASK_COUNT] ={
		[TASK_PRINTF] = 		DEFINE_TASK(TASK_PRINTF, 			PRIORITY_HIGH, 			process_print_f, 			PERIOD_US_FROM_HERTZ(20)),
		[TASK_LED] = 			DEFINE_TASK(TASK_LED, 				PRIORITY_LOW,	 		process_led, 				PERIOD_US_FROM_HERTZ(10)),
		[TASK_GYRO_UPDATE] = 	DEFINE_TASK(TASK_GYRO_UPDATE, 		PRIORITY_HIGH,	 		process_gyro_update, 		PERIOD_US_FROM_HERTZ(20)),
		[TASK_ACC_UPDATE] = 	DEFINE_TASK(TASK_ACC_UPDATE, 		PRIORITY_HIGH,	 		process_acc_update, 		PERIOD_US_FROM_HERTZ(20))
};

task_t * TASK_get_task(task_ids_t id){
	return &tasks[id];
}


