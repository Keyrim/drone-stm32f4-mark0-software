/*
 * baro.c
 *
 *  Created on: 2 déc. 2020
 *      Author: Théo
 */

#include "baro.h"
#include "../OS/events/events.h"


static void ms5611_raw_temp_rdy();
static void ms5611_raw_pressure_rdy();

static void ms5611_raw_temp_rdy(){
	EVENT_Set_flag(FLAG_BARO_TEMP_RAW_RDY);
}
static void ms5611_raw_pressure_rdy(){
	EVENT_Set_flag(FLAG_BARO_PRESSURE_RAW_RDY);
}

void BARO_init(baro_t * baro, ms5611_t * ms5611, I2C_HandleTypeDef * hi2c){
	baro->ms5611 = ms5611 ;

	switch(MS5611_init(baro->ms5611, hi2c, ms5611_raw_temp_rdy, ms5611_raw_pressure_rdy))
	{
		case SENSOR_REQUEST_OK :
			EVENT_Set_flag(FLAG_BARO_OK);
			break;
		case SENSOR_REQUEST_FAIL :
			break;
		case SENSOR_REQUEST_BUSY :
			break;
	}
}

uint32_t BARO_Main(baro_t * baro){
	uint32_t to_return = 9600 ;
	switch(baro->state){
		case BARO_STATE_IDDLE:
			baro->state = BARO_STATE_ASK_TEMP ;
			break;
		case BARO_STATE_ASK_TEMP :
			MS5611_request_temp(baro->ms5611);
			baro->state = BARO_STATE_READ_TEMP ;
			break;
		case BARO_STATE_READ_TEMP :
			MS5611_read_temp(baro->ms5611);
			baro->state = BARO_STATE_ASK_PRESSURE ;
			to_return = 400 ;
			break;
		case BARO_STATE_ASK_PRESSURE :
			MS5611_request_pressure(baro->ms5611);
			baro->state = BARO_STATE_READ_PRESSURE;
			break;
		case BARO_STATE_READ_PRESSURE :
			MS5611_read_pressure(baro->ms5611);
			baro->state = BARO_STATE_ASK_TEMP ;
			to_return = 400 ;
			break;
	}
	return to_return ;
}

void BARO_compute_temp(baro_t * baro){
	MS5611_calculate_temperature(baro->ms5611);

}
void BARO_compute_pressure(baro_t * baro){
	MS5611_calculate_pressure(baro->ms5611);
}
void BARO_compute_altitude(baro_t * baro){
	MS5611_calculate_altitude(baro->ms5611);
}


















