/*
 * Uart_DMA.h
 *
 *  Created on: Dec 8, 2020
 *      Author: Théo
 */

#ifndef INC_UART_DMA_H_
#define INC_UART_DMA_H_

#include "stm32f4xx_hal.h"

typedef struct uart_t{
	UART_HandleTypeDef * huart ;
}uart_t;



#endif /* INC_UART_DMA_H_ */
