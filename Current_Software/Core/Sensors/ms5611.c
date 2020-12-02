/*
 * ms5611.c
 *
 *  Created on: 2 déc. 2020
 *      Author: Théo
 */

#include "ms5611.h"

sensor_state_e MS5611_init(ms5611_t * ms5611, I2C_HandleTypeDef * hi2c){
	ms5611->state = SENSOR_NOT_INIT ;

	if(hi2c == NULL)
		return SENSOR_ERROR ;

	ms5611->hi2c = hi2c ;
	ms5611->hal_state = HAL_I2C_IsDeviceReady(ms5611->hi2c, MS5611_ADRESSE, 5, 2);
	if(ms5611->hal_state != HAL_OK)
		return SENSOR_ERROR ;

	//Get calibrations values from the sensor
	uint8_t data[2];
	for(uint8_t d = 1; d < 7; d++){
		if(ms5611->hal_state == HAL_OK){
			ms5611->hal_state = HAL_I2C_Mem_Read(ms5611->hi2c, MS5611_ADRESSE, (uint8_t)(MS5611_PROM + (2*d)), I2C_MEMADD_SIZE_8BIT, data, 2, 2);
			ms5611->calibration_values[d-1] = (uint16_t)((data[0] << 8) | data[1]);
		}
		else
			return SENSOR_ERROR ;
	}

	return SENSOR_IDDLE ;

}
