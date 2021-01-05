/*
 * Data_logger.h
 *
 *  Created on: 22 déc. 2020
 *      Author: theom
 */

#ifndef DATA_LOGGER_DATA_LOGGER_H_
#define DATA_LOGGER_DATA_LOGGER_H_

#include "../OS/system_d.h"

typedef enum data_id_e{
	DATA_ID_ROLL_ANGLE,
	DATA_ID_PITCH_ANGLE,
	DATA_ID_ROLL_GYRO,
	DATA_ID_PITCH_GYRO,
	DATA_ID_YAW_GYRO,
	DATA_ID_ROLL_GYRO_RAW,
	DATA_ID_PITCH_GYRO_RAW,
	DATA_ID_YAW_GYRO_RAW,
	DATA_ID_MOTOR_1,

	DATA_ID_CONFIG_REQUEST,		//Si je reçois ça, j'envoit la configuration des données

	DATA_ID_COUNT
}data_id_e;

typedef enum data_format_e{
	//O octet
	DATA_FORMAT_0B_BUTTON = 0,	//Envoyé par un autre périphérique => déclenche une action sur le drone potentiellement
	//1 octet
	DATA_FORMAT_8B = 20,
	DATA_FORMAT_8B_PWM,
	DATA_FORMAT_8B_FLOAT_1D,
	DATA_FORMAT_8B_FLOAT_0D,
	//2 octets
	DATA_FORMAT_16B = 40,
	DATA_FORMAT_16B_FLOAT_1D,
}data_format_e;


typedef struct data_t{
	uint8_t len;
	uint8_t * data;
	uint8_t id;
	data_format_e format;
	uint8_t * name ;
	uint8_t len_name;
	bool_e used;
}data_t;


void DATA_LOGGER_Init(system_t * sys_);
void DATA_LOGGER_Main(void);
void DATA_LOGGER_Reception(uint8_t * input_buffer);
data_t * DATA_LOGGER_Get_Data(data_id_e id);
uint8_t DATA_LOGGER_Get_Data_Value(data_id_e id, uint8_t * buffer);
uint8_t DATA_LOGGER_Get_Data_Config(data_id_e id, uint8_t * buffer);

#endif /* DATA_LOGGER_DATA_LOGGER_H_ */