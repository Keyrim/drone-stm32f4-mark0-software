/*
 * Telemetry.h
 *
 *  Created on: Dec 18, 2020
 *      Author: Théo
 */

#ifndef TELEMETRY_TELEMETRY_H_
#define TELEMETRY_TELEMETRY_H_



#include "stm32f4xx_hal.h"

//Envoit max de 2000 octets par secondes si utilisation de la radio sinon ça déconne ..

#define USE_RADIO 1

#define SIZE_PORTION_BUFFER 20
#define NB_PORTION_BUFFER	20

typedef struct telemetry_t{
	UART_HandleTypeDef * huart;
	HAL_StatusTypeDef hal_state ;
	uint8_t buffer_tx[SIZE_PORTION_BUFFER*NB_PORTION_BUFFER];
	uint8_t len_portion_tx[NB_PORTION_BUFFER];
	uint32_t next_send ;
	uint16_t index_write_tx ;
	uint16_t index_read_tx ;
}telemetry_t;


void TELEMETRY_Init(telemetry_t * telem, UART_HandleTypeDef * huart);
void TELEMETRY_Process(telemetry_t * telem, uint32_t time_us);
void TELEMETRY_Send_Data(telemetry_t * telem, uint8_t * data, uint8_t len);


#endif /* TELEMETRY_TELEMETRY_H_ */
