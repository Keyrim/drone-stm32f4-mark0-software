/*
 * mask_def.c
 *
 *  Created on: 17 sept. 2020
 *      Author: Theo
 */

#include "mask_def.h"



//Prototypes
// ----------------	GYRO ------------------------
static void mask_def_gyro_dma_done(Event_t * event);
static void mask_def_gyro_init_ok(Event_t * event);



//----------------	GLOBAL INIT ------------------------
void mask_def_events_it_init(Event_t * event){
	//Gyro mask def
	mask_def_gyro_dma_done(&event[EVENT_IT_GYRO_DMA_DONE]);

}

void mask_def_events_main_init(Event_t * event){
	//Gyro mask def
	mask_def_gyro_init_ok(&event[EVENT_MAIN_GYRO_INIT_OK]);
}

// ----------------	GYRO ------------------------
static void mask_def_gyro_dma_done(Event_t * event){
	//Pas de conditions, si le flag est présent on déclenche ^^
	MASK_set_flag(&event->mask_and[MASK_GYRO_DMA_DONE_DMA_IT], FLAG_GYRO_DMA_DONE);
	MASK_set_flag(&event->mask_or[MASK_GYRO_DMA_DONE_DMA_IT], FLAG_GYRO_DMA_DONE);
}
static void mask_def_gyro_init_ok(Event_t * event){
	//Si le gyro est init et pas déjà en train de tourner
	MASK_set_flag(&event->mask_and[MASK_GYRO_INIT_OK], FLAG_GYRO_INIT);
	MASK_set_flag(&event->mask_or[MASK_GYRO_INIT_OK], FLAG_GYRO_INIT);
	MASK_set_flag(&event->mask_not[MASK_GYRO_INIT_OK], FLAG_GYRO_READING);
}










