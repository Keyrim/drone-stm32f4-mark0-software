/*
 * sensors.h
 *
 *  Created on: Nov 22, 2020
 *      Author: Théo
 */

#ifndef SENSORS_SENSORS_H_
#define SENSORS_SENSORS_H_


typedef enum sensor_connectivity_e{
	USE_UART,
	USE_UART_DMA,
	USE_I2C,
	USE_I2C_DMA
}sensor_connectivity_e;


typedef enum sensor_state_e{
	SENSOR_IDDLE,
	SENSOR_NOT_INIT,
	SENSOR_NOT_DETECTED,
	SENSOR_IN_PROGRESS,
	SENSOR_HAL_BUSY,
	SENSOR_ERROR
}sensor_state_e;


typedef enum gyro_axe_e{
	GYRO_AXE_X,
	GYRO_AXE_Y,
	GYRO_AXE_Z
}gyro_axe_e;


#endif /* SENSORS_SENSORS_H_ */
