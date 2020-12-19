/*
 * Telemetry.c
 *
 *  Created on: Dec 18, 2020
 *      Author: Théo
 */

#include "Telemetry.h"

//static uint8_t test_msg[] = "Hey it s me Theo...\n";

void TELEMETRY_Init(telemetry_t * telem, UART_HandleTypeDef * huart)
{
	telem->huart = huart ;

}
void TELEMETRY_Process(telemetry_t * telem,  uint32_t time_us){
	if(telem->index_write_tx != telem->index_read_tx)
	{
		//On transmet les données
		HAL_UART_Transmit_DMA(telem->huart, &telem->buffer_tx[SIZE_PORTION_BUFFER*telem->index_read_tx], telem->len_portion_tx[telem->index_read_tx]);
		//Incrémentation de l'index
		telem->index_read_tx ++ ;
		telem->index_read_tx = telem->index_write_tx % NB_PORTION_BUFFER ;
	}
}

void TELEMETRY_Send_Data(telemetry_t * telem, uint8_t * data, uint8_t len){

	//Copie du tableau dans le buffer
	for(uint8_t c = 0; c < len; c++)
		telem->buffer_tx[telem->index_write_tx*SIZE_PORTION_BUFFER + c] = data[c];
	//On enregistre la taille de la donnée
	telem->len_portion_tx[telem->index_write_tx] = len ;

	//Incrémentation de l'index
	telem->index_write_tx ++ ;
	telem->index_write_tx = telem->index_write_tx % NB_PORTION_BUFFER ;
}
