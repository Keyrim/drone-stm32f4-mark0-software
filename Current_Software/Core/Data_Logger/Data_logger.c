/*
 * Data_logger.c
 *
 *  Created on: 22 déc. 2020
 *      Author: theom
 */

#include "Data_logger.h"
#include "string.h"
#include "../OS/tasks/task.h"
#include "../OS/scheduler/scheduler.h"
static system_t * sys ;





#define DEFINE_DATA(id_, ptr_, format_, name_, use_) {	\
	data_list[id_].id = id_ ;			\
	data_list[id_].data = ptr_ ;		\
	data_list[id_].format = format_;	\
	data_list[id_].name = name_;			\
	data_list[id_].len_name = sizeof(name_)-1;	\
	data_list[id_].used = use_;				\
}


typedef enum data_logger_state_e{
	LOGGER_IDDLE,
	LOGGER_TRANSMIT_CONFIG,
	LOGGER_LOG
}data_logger_state_e;


static data_t data_list[DATA_ID_COUNT] ;
static data_logger_state_e state = LOGGER_IDDLE;
static bool_e config_not_sent = TRUE ;
static bool_e start_flag = FALSE;
static bool_e stop_flag = FALSE;
static uint8_t tmp[20];
static uint8_t tmp_len ;
static uint8_t id_init_counter = 0;
static bool_e is_any_data_used = FALSE;

//Data name def
static uint8_t name_roll_angle[] = "Angle ROLL";
static uint8_t name_pitch_angle[] = "Angle PITCH";

static uint8_t name_consigne_angle_roll[] = "Target ROLL";
static uint8_t name_consigne_angle_pitch[] = "Target PITCH";

static uint8_t name_roll_gyro [] = "Gyro ROLL";
static uint8_t name_pitch_gyro[] = "Gyro PITCH";
static uint8_t name_yaw_gyro[] = "Gyro YAW";

static uint8_t name_target_roll_gyro[] = "Tar Gyro ROLL";
static uint8_t name_target_pitch_gyro[] = "Tar Gyro PITCH";
static uint8_t name_target_yaw_gyro[] = "Tar Gyro YAW";


static uint8_t name_roll_gyro_raw [] = "Gyro raw ROLL";
static uint8_t name_pitch_gyro_raw[] = "Gyro raw PITCH";
static uint8_t name_yaw_gyro_raw[] = "Gyro raw YAW";

static uint8_t name_pid_gyro_roll[] = "PID Gyro Roll";
static uint8_t name_pid_gyro_roll_p[] = "PID Gyro Ro P";
static uint8_t name_pid_gyro_roll_d[] = "PID Gyro Ro D";

static uint8_t name_prop_thrust[] = "PROP Thrust";

static uint8_t name_config_request[] = "Send Config";
static uint8_t name_enable_asser_orientation[] = "En Asser Ori";
static uint8_t name_disable_asser_orientation[] = "Dis Asser Ori";
static uint8_t name_start_transfer[] = "Start Transfer";
static uint8_t name_stop_transfer[] = "Stop Transfer";

static uint8_t name_flight_mode[] = "Flight Mode";

void DATA_LOGGER_Init(system_t * sys_){

	sys = sys_;

	//	-----------------------------------------------	Définitions des data	-----------------------------------------------------------------
	//Angle
	DEFINE_DATA(DATA_ID_ROLL_ANGLE, (uint8_t*)&sys->orientation.angular_position[ORIENTATION_ROLL], 							DATA_FORMAT_16B_FLOAT_1D, 	name_roll_angle,							TRUE);
	DEFINE_DATA(DATA_ID_PITCH_ANGLE, (uint8_t*)&sys->orientation.angular_position[ORIENTATION_PITCH], 							DATA_FORMAT_16B_FLOAT_1D, 	name_pitch_angle,							FALSE);

	//Consignes angles
	DEFINE_DATA(DATA_ID_CONSIGNE_ANGLE_ROLL, (uint8_t*)&sys->regulation.orientation.consigne_angular_pos[ORIENTATION_ROLL], 	DATA_FORMAT_16B_FLOAT_1D, 	name_consigne_angle_roll,					TRUE);
	DEFINE_DATA(DATA_ID_CONSIGNE_ANGLE_PITCH, (uint8_t*)&sys->regulation.orientation.consigne_angular_pos[ORIENTATION_PITCH], 	DATA_FORMAT_16B_FLOAT_1D, 	name_consigne_angle_pitch,					FALSE);

	//Pid gyro roll
	DEFINE_DATA(DATA_ID_PID_GYRO_ROLL, (uint8_t*)&sys->regulation.orientation.pid_angular_speed[ORIENTATION_ROLL].output, 		DATA_FORMAT_16B_FLOAT_1D, 	name_pid_gyro_roll, 						FALSE);
	DEFINE_DATA(DATA_ID_PID_GYRO_ROLL_P, (uint8_t*)&sys->regulation.orientation.pid_angular_speed[ORIENTATION_ROLL].P, 			DATA_FORMAT_16B_FLOAT_1D, 	name_pid_gyro_roll_p, 						FALSE);
	DEFINE_DATA(DATA_ID_PID_GYRO_ROLL_D, (uint8_t*)&sys->regulation.orientation.pid_angular_speed[ORIENTATION_ROLL].D, 			DATA_FORMAT_16B_FLOAT_1D, 	name_pid_gyro_roll_d, 						FALSE);

	//Angle rate
	DEFINE_DATA(DATA_ID_ROLL_GYRO, (uint8_t*)&sys->sensors.gyro.filtered[ORIENTATION_ROLL], 									DATA_FORMAT_16B_FLOAT_1D, 	name_roll_gyro, 								TRUE);
	DEFINE_DATA(DATA_ID_PITCH_GYRO, (uint8_t*)&sys->sensors.gyro.filtered[ORIENTATION_PITCH], 									DATA_FORMAT_16B_FLOAT_1D, 	name_pitch_gyro, 								FALSE);
	DEFINE_DATA(DATA_ID_YAW_GYRO, (uint8_t*)&sys->sensors.gyro.filtered[ORIENTATION_YAW], 										DATA_FORMAT_16B_FLOAT_1D, 	name_yaw_gyro, 			 						FALSE);

	//Consignes angles rates
	DEFINE_DATA(DATA_ID_CONSIGNE_GYRO_ROLL, (uint8_t*)&sys->regulation.orientation.consigne_angular_speed[ORIENTATION_ROLL], 	DATA_FORMAT_16B_FLOAT_1D, 	name_target_roll_gyro, 							TRUE);
	DEFINE_DATA(DATA_ID_CONSIGNE_GYRO_PITCH, (uint8_t*)&sys->regulation.orientation.consigne_angular_speed[ORIENTATION_PITCH], 	DATA_FORMAT_16B_FLOAT_1D, 	name_target_pitch_gyro, 						FALSE);
	DEFINE_DATA(DATA_ID_CONSIGNE_GYRO_YAW, (uint8_t*)&sys->regulation.orientation.consigne_angular_speed[ORIENTATION_YAW], 		DATA_FORMAT_16B_FLOAT_1D, 	name_target_yaw_gyro, 		 					FALSE);

	//Angle Rate raw
	DEFINE_DATA(DATA_ID_ROLL_GYRO_RAW, (uint8_t*)&sys->sensors.gyro.raw[ORIENTATION_ROLL], 										DATA_FORMAT_16B_FLOAT_1D, 	name_roll_gyro_raw, 							FALSE);
	DEFINE_DATA(DATA_ID_PITCH_GYRO_RAW, (uint8_t*)&sys->sensors.gyro.raw[ORIENTATION_ROLL], 									DATA_FORMAT_16B_FLOAT_1D, 	name_pitch_gyro_raw,							FALSE);
	DEFINE_DATA(DATA_ID_YAW_GYRO_RAW, (uint8_t*)&sys->sensors.gyro.raw[ORIENTATION_YAW], 										DATA_FORMAT_16B_FLOAT_1D, 	name_yaw_gyro_raw, 								FALSE);

	//Buttons
	DEFINE_DATA(DATA_ID_CONFIG_REQUEST, NULL, 																					DATA_FORMAT_0B_BUTTON, 		name_config_request, 							FALSE);
	DEFINE_DATA(DATA_ID_DISABLE_ASSER_ORIENTATION, NULL, 																		DATA_FORMAT_0B_BUTTON, 		name_disable_asser_orientation,  	TRUE);
	DEFINE_DATA(DATA_ID_ENABLE_ASSER_ORIENTATION, NULL, 																		DATA_FORMAT_0B_BUTTON, 		name_enable_asser_orientation, 		TRUE);
	DEFINE_DATA(DATA_ID_START_TRANSFER, NULL, 																					DATA_FORMAT_0B_BUTTON, 		name_start_transfer, 		 					TRUE);
	DEFINE_DATA(DATA_ID_STOP_TRANSFER, NULL, 																					DATA_FORMAT_0B_BUTTON, 		name_stop_transfer, 							TRUE);

	//Propulsion
	DEFINE_DATA(DATA_ID_PROP_THRUST, (uint8_t*)&sys->propulsion.consigne[PROP_CONSIGNE_THRUST], 								DATA_FORMAT_16B_UINT16,  	name_prop_thrust, 								FALSE);


	//Others
	DEFINE_DATA(DATA_ID_FLIGHT_MODE, sys->soft.flight_mode, 																	DATA_FORMAT_8B, 			name_flight_mode, 								FALSE);


}

void DATA_LOGGER_Start(){
	start_flag = TRUE;
}

void DATA_LOGGER_Stop(){
	stop_flag = TRUE;
}

void DATA_LOGGER_Main(void){

	static data_logger_state_e previous_state 	= LOGGER_TRANSMIT_CONFIG ;
	bool_e entrance = previous_state != state ;
	previous_state = state ;

	switch(state){
		case LOGGER_IDDLE :
			if(entrance){
				//Quand on passe en mode iddle en envoit l'id du boutton de config toute les 2 secondes
				SCHEDULER_reschedule_task(TASK_LOGGER, 2000000);
			}

			tmp_len = DATA_LOGGER_Get_Data_Config(DATA_ID_CONFIG_REQUEST, tmp);
			TELEMETRY_Send_Data(tmp, tmp_len);

			//Si on a reçu une requête pour envoyer la config, on y go
			if(start_flag){
				//Reset flags
				start_flag = FALSE;
				stop_flag = FALSE;
				//Change the state
				if(config_not_sent){
					id_init_counter = 0;
					is_any_data_used = FALSE;
					state = LOGGER_TRANSMIT_CONFIG;

				}
				else
					state = LOGGER_LOG;
			}
			break;
		case LOGGER_TRANSMIT_CONFIG :

			if(entrance)
				SCHEDULER_reschedule_task(TASK_LOGGER, 500000);	//On abaisse la fréquence de la tâche pendant l'init

			//If the data[id_init_counter] is used
			while(!data_list[id_init_counter].used && id_init_counter < DATA_ID_COUNT){
				id_init_counter ++ ;
			}
			if(id_init_counter < DATA_ID_COUNT){
				is_any_data_used = TRUE;
				//Get a data config
				tmp_len = DATA_LOGGER_Get_Data_Config(id_init_counter++, tmp);
				//Send the data config
				TELEMETRY_Send_Data(tmp, tmp_len);
			}

			else
			{
				if(is_any_data_used)
				{

					config_not_sent = FALSE ;
					state = LOGGER_LOG ;
				}
				else
					state = LOGGER_IDDLE ;

			}

			break;

		case LOGGER_LOG:
			if(entrance)
				SCHEDULER_reschedule_task(TASK_LOGGER, 10000);	//On reprend une fréquence plus importante pour l'envoit en flux continu

			if(stop_flag)
				state = LOGGER_IDDLE;

			for(uint8_t d = 0; d < DATA_ID_COUNT; d++)
			{
				if(data_list[d].used && (data_list[d].format != DATA_FORMAT_0B_BUTTON))
				{
					tmp_len = DATA_LOGGER_Get_Data_Value(d, tmp);
					TELEMETRY_Send_Data(tmp, tmp_len);
				}
			}
			break;
	}

}

void DATA_LOGGER_Reception(uint8_t * input_buffer){
	uint8_t id = input_buffer[0];

	switch(data_list[id].format){
		case DATA_FORMAT_0B_BUTTON:
			//We can create an action for each id
			switch(id)
			{
				default:
					break;
				case DATA_ID_CONFIG_REQUEST:
					start_flag = TRUE;
					break;
				case DATA_ID_ENABLE_ASSER_ORIENTATION:
					REGULATION_ORIENTATION_Set_Regulation_Mode(REGULATION_ORIENTATION_MODE_ACCRO);
					break;
				case DATA_ID_DISABLE_ASSER_ORIENTATION:
					REGULATION_ORIENTATION_Set_Regulation_Mode(REGULATION_ORIENTATION_MODE_OFF);
					break;
				case DATA_ID_START_TRANSFER:
					DATA_LOGGER_Start();
					break;
				case DATA_ID_STOP_TRANSFER:
					DATA_LOGGER_Stop();
					break;
			}
			break;
		case DATA_FORMAT_8B:
			break;
		case DATA_FORMAT_8B_FLOAT_0D:
			break;
		case DATA_FORMAT_8B_FLOAT_1D:
			break;
		case DATA_FORMAT_8B_PWM:
			break;
		case DATA_FORMAT_16B:
			break;
		case DATA_FORMAT_16B_FLOAT_1D:
			break;
		default:
			break;

	}
}

uint8_t DATA_LOGGER_Get_Data_Value(data_id_e id, uint8_t * buffer){

//	uint16_t tmp_u16 = 0;
	int16_t tmp_16 = 0;
	float tmp_float = 0;

	//Premier octet contient l'id
	buffer[0] = id ;

	//Formatage de la donnée en fonction du type et du format
	switch(data_list[id].format){
		case DATA_FORMAT_8B:
			buffer[1] = *data_list[id].data ;
			break;
		case DATA_FORMAT_8B_PWM:
			break;
		case DATA_FORMAT_8B_FLOAT_0D:
			tmp_float = *(float*)data_list[id].data ;
			buffer[1] = (int8_t)tmp_float;
			break;
		case DATA_FORMAT_8B_FLOAT_1D:
			break;
		case DATA_FORMAT_16B:
			break;
		case DATA_FORMAT_16B_FLOAT_1D:
			tmp_float = *(float*)data_list[id].data;
			tmp_16 = tmp_float * 10;
			buffer[1] = (uint8_t)((tmp_16 >> 8) & 0b11111111) ;
			buffer[2] = (uint8_t)(tmp_16 & 0b11111111);
			break;
		case DATA_FORMAT_16B_UINT16:
			tmp_16 = *(uint16_t*)data_list[id].data ;
			buffer[1] = (uint8_t)((tmp_16 >> 8) & 0b11111111) ;
			buffer[2] = (uint8_t)(tmp_16 & 0b11111111);
			break;
		case DATA_FORMAT_16B_INT16:
			tmp_16 = *(int16_t*)data_list[id].data ;
			buffer[1] = (uint8_t)((tmp_16 >> 8) & 0b11111111) ;
			buffer[2] = (uint8_t)(tmp_16 & 0b11111111);
			break;
		default:
			break;
	}

	//Return the len of data
	return (data_list[id].format / 20) + 1;

}

uint8_t DATA_LOGGER_Get_Data_Config(data_id_e id, uint8_t * buffer){
	//New data so we start with 255
	buffer[0] = 255;
	//Then name len
	buffer[1] = data_list[id].len_name;
	//Then format
	buffer[2] = data_list[id].format ;
	//Taille du nom
	buffer[3] = id	 ;
	//Copie du nom
	memcpy(&buffer[4], data_list[id].name, data_list[id].len_name);

	//And that s it
	return 4 + data_list[id].len_name ;
}

data_t * DATA_LOGGER_Get_Data(data_id_e id){
	return &data_list[id];
}



