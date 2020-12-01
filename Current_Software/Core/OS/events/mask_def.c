/*
 * mask_def.c
 *
 *  Created on: 17 sept. 2020
 *      Author: Theo
 */

#include "mask_def.h"



void mask_def_gyro_dma_done(Event_t * event){
	//Pas de conditions, si le flag est présent on déclenche ^^
	MASK_set_flag(&event->mask_and[MASK_GYRO_DMA_DONE_DMA_IT], FLAG_DMA_GYRO_DONE);
	MASK_set_flag(&event->mask_or[MASK_GYRO_DMA_DONE_DMA_IT], FLAG_DMA_GYRO_DONE);
}












