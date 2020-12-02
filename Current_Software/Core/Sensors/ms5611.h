/*
 * ms5611.h
 *
 *  Created on: 2 déc. 2020
 *      Author: Théo
 */

#ifndef SENSORS_MS5611_H_
#define SENSORS_MS5611_H_

#include "sensors.h"

#define MS5611_ADRESSE  0xEE

/*  MS5611 registers */
#define MS5611_PROM                 0xA0
#define MS5611_TEMP                 0X58
#define MS5611_PRESSURE             0X48
#define MS5611_READ_DATA            0X00

typedef struct ms5611_t{

	I2C_HandleTypeDef * hi2c ;
	HAL_StatusTypeDef hal_state ;


	sensor_state_e state ;

	uint16_t calibration_values[6];


}ms5611_t;


sensor_state_e MS5611_init(ms5611_t * ms5611, I2C_HandleTypeDef * hi2c);

#endif /* SENSORS_MS5611_H_ */
