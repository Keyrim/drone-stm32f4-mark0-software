/*
 * mpu.c
 *
 *  Created on: 26 nov. 2020
 *      Author: Théo
 */

#include "mpu.h"
#include "../OS/time.h"

//Private functions
void convert_gyro(mpu_t * mpu){
#ifdef MPU_USE_I2C
	mpu->gyro_raw[GYRO_AXE_X] = (int16_t)(mpu->gyro_data[0] << 8 | mpu->gyro_data[1]);
	mpu->gyro_raw[GYRO_AXE_Y] = (int16_t)(mpu->gyro_data[2] << 8 | mpu->gyro_data[3]);
	mpu->gyro_raw[GYRO_AXE_Z] = (int16_t)(mpu->gyro_data[4] << 8 | mpu->gyro_data[5]);

#else
	#ifdef MPU_USE_SPI
	mpu->gyro_raw[GYRO_AXE_X] = (int16_t)(mpu->gyro_data[1] << 8 | mpu->gyro_data[0]);
	mpu->gyro_raw[GYRO_AXE_Y] = (int16_t)(mpu->gyro_data[3] << 8 | mpu->gyro_data[2]);
	mpu->gyro_raw[GYRO_AXE_Z] = (int16_t)(mpu->gyro_data[5] << 8 | mpu->gyro_data[4]);
	#endif
#endif
	//Convertion
	mpu->gyro[GYRO_AXE_X] = (float)mpu->gyro_raw[GYRO_AXE_X] * mpu->gyro_sensi ;
	mpu->gyro[GYRO_AXE_Y] = (float)mpu->gyro_raw[GYRO_AXE_Y] * mpu->gyro_sensi ;
	mpu->gyro[GYRO_AXE_Z] = (float)mpu->gyro_raw[GYRO_AXE_Z] * mpu->gyro_sensi ;
}

void convert_acc(mpu_t * mpu){
#ifdef MPU_USE_I2C
	mpu->acc_raw[ACC_AXE_X] = (int16_t)(mpu->acc_data[0] << 8 | mpu->acc_data[1]);
	mpu->acc_raw[ACC_AXE_Y] = (int16_t)(mpu->acc_data[2] << 8 | mpu->acc_data[3]);
	mpu->acc_raw[ACC_AXE_Z] = (int16_t)(mpu->acc_data[4] << 8 | mpu->acc_data[5]);

#else
	#ifdef MPU_USE_SPI
	mpu->acc_raw[ACC_AXE_X] = (int16_t)(mpu->acc_data[1] << 8 | mpu->acc_data[0]);
	mpu->acc_raw[ACC_AXE_Y] = (int16_t)(mpu->acc_data[3] << 8 | mpu->acc_data[2]);
	mpu->acc_raw[ACC_AXE_Z] = (int16_t)(mpu->acc_data[5] << 8 | mpu->acc_data[4]);
	#endif
#endif
	//Convertion
	mpu->acc[ACC_AXE_X] = (float)mpu->acc_raw[ACC_AXE_X] * mpu->acc_sensi ;
	mpu->acc[ACC_AXE_Y] = (float)mpu->acc_raw[ACC_AXE_Y] * mpu->acc_sensi ;
	mpu->acc[ACC_AXE_Z] = (float)mpu->acc_raw[ACC_AXE_Z] * mpu->acc_sensi ;
}

void MPU_cs_lock(mpu_t * mpu){
	HAL_GPIO_WritePin(mpu->pin_cs_gpio, mpu->pin_cs, RESET);
}

void MPU_cs_unlock(mpu_t * mpu){
	HAL_GPIO_WritePin(mpu->pin_cs_gpio, mpu->pin_cs, SET);
}

/*
 *
 */
sensor_state_e MPU_init(mpu_t * mpu, I2C_HandleTypeDef * hi2c, SPI_HandleTypeDef * hspi, GPIO_TypeDef * pin_cs_gpio, uint16_t pin_cs){
	//Etat par default
	mpu->state = SENSOR_NOT_INIT ;



	//Requiert un I2C ou SPI pour fonctionner
	if(hi2c == NULL && hspi == NULL)
		return mpu->state ;

	//Application des paramètres de connexion
	mpu->hi2c = hi2c ;
	mpu->hspi = hspi ;
	mpu->pin_cs = pin_cs ;
	mpu->pin_cs_gpio = pin_cs_gpio ;

	mpu->adresse = MPU6050_I2C_ADDR ;

	//Check la présence du mpu si I2C
#ifdef MPU_USE_I2C
	mpu->hal_state = HAL_I2C_IsDeviceReady(mpu->hi2c, mpu->adresse, 2, 2);
	if(mpu->hal_state != HAL_OK){
		mpu->state = SENSOR_NOT_DETECTED ;
		return mpu->state ;
	}
#endif


	//Wakeup du mpu

#ifdef MPU_USE_I2C
	uint8_t wakeup_data = 0x00 ;
	mpu->hal_state  = HAL_I2C_Mem_Write(mpu->hi2c, mpu->adresse, MPU6050_PWR_MGMT_1, I2C_MEMADD_SIZE_8BIT, &wakeup_data, 1, 10);
#endif
#ifdef	MPU_USE_SPI
	uint8_t wakeup_data[] = {MPU6050_PWR_MGMT_1, 0x00} ;

	MPU_cs_lock(mpu);
	mpu->hal_state = HAL_SPI_Transmit(mpu->hspi, wakeup_data, 2, 2);
	MPU_cs_unlock(mpu);
#endif

	if(mpu->hal_state != HAL_OK){
		mpu->state = SENSOR_ERROR ;
		return mpu->state ;
	}


	//Si on réussi tous les test, on dit qu'on est bueno
	mpu->state = SENSOR_IDDLE ;
	return mpu->state ;
}

/*
 *
 */
sensor_state_e MPU_init_gyro(mpu_t * mpu, MPU_gyro_range_e gyro_range){
	mpu->gyro_range = gyro_range ;

	//Si mpu non utilisable
	if(mpu->state != SENSOR_IDDLE)
		return SENSOR_ERROR ;

	//Configuration de la sensi du gyro dans le mpu
#ifdef MPU_USE_I2C
	uint8_t temp ;
	mpu->hal_state = HAL_I2C_Mem_Read(mpu->hi2c, mpu->adresse, MPU6050_GYRO_CONFIG, I2C_MEMADD_SIZE_8BIT, &temp, 1, 2);
	temp = (temp & 0xE7) | (uint8_t)gyro_range << 3;
	if(mpu->hal_state == HAL_OK)
		mpu->hal_state = HAL_I2C_Mem_Write(mpu->hi2c, mpu->adresse, MPU6050_GYRO_CONFIG, I2C_MEMADD_SIZE_8BIT, &temp, 1, 2);

#else
	#ifdef MPU_USE_SPI
	uint8_t temp[2] ;
	temp[0] = MPU6050_GYRO_CONFIG | MPU6050_READ ;
	MPU_cs_lock(mpu);
	mpu->hal_state = HAL_SPI_TransmitReceive(mpu->hspi, temp, temp, 2, 2);
	MPU_cs_unlock(mpu);
	if(mpu->hal_state == HAL_OK){
		HAL_Delay(1);
		MPU_cs_lock(mpu);
		temp[0] = MPU6050_GYRO_CONFIG ;
		temp[1] = (temp[1] & 0xE7) | (uint8_t)gyro_range << 3;
		mpu->hal_state = HAL_SPI_Transmit(mpu->hspi, temp,  2, 2);
		MPU_cs_unlock(mpu);
	}
	if(mpu->hal_state == HAL_OK){ // on vérifi si on a bien configuré le gyro comme on voulait
		uint8_t temp2 = temp[1];
		temp[0] = MPU6050_GYRO_CONFIG | MPU6050_READ ;
		HAL_Delay(1);
		MPU_cs_lock(mpu);
		mpu->hal_state = HAL_SPI_TransmitReceive(mpu->hspi, temp, temp, 2, 2);
		MPU_cs_unlock(mpu);

		if(temp[1] != temp2)
			return SENSOR_NOT_INIT ;
	}


	#endif
#endif

	if(mpu->hal_state != HAL_OK)
	{
		mpu->state = SENSOR_ERROR ;
		return SENSOR_ERROR ;
	}

	switch(gyro_range){
		case MPU_GYRO_250s :
			mpu->gyro_sensi = (float)1 / MPU6050_GYRO_SENS_250 ;
			break;
		case MPU_GYRO_500s :
			mpu->gyro_sensi = (float)1 / MPU6050_GYRO_SENS_500 ;
			break;
		case MPU_GYRO_1000s :
			mpu->gyro_sensi = (float)1 / MPU6050_GYRO_SENS_1000 ;
			break;
		case MPU_GYRO_2000s :
			mpu->gyro_sensi = (float)1 / MPU6050_GYRO_SENS_2000 ;
			break;
	}
	return SENSOR_IDDLE;
}

/*
 *
 */
sensor_state_e MPU_init_acc(mpu_t * mpu, MPU_acc_range_e acc_range){
	mpu->acc_range = acc_range ;

	//Si mpu non utilisable
	if(mpu->state != SENSOR_IDDLE)
		return SENSOR_ERROR ;


	//Configuration de la sensi de l'acc dans le mpu
#ifdef MPU_USE_I2C
	uint8_t temp ;
	HAL_I2C_Mem_Read(mpu->hi2c, mpu->adresse, MPU6050_ACCEL_CONFIG, I2C_MEMADD_SIZE_8BIT, &temp, 1, 2);
	temp = (temp & 0xE7) | (uint8_t)acc_range << 3;
	HAL_I2C_Mem_Write(mpu->hi2c, mpu->adresse, MPU6050_ACCEL_CONFIG, I2C_MEMADD_SIZE_8BIT, &temp, 1, 2);
#else
	#ifdef MPU_USE_SPI
	uint8_t temp[2] ;
	temp[0] = MPU6050_ACCEL_CONFIG | MPU6050_READ ;
	MPU_cs_lock(mpu);
	mpu->hal_state = HAL_SPI_TransmitReceive(mpu->hspi, temp, temp, 2, 2);
	MPU_cs_unlock(mpu);
	if(mpu->hal_state == HAL_OK){	//Si mpu ok on écrit en spi dans le registre de config du mpu dédié au gyro
		HAL_Delay(1);
		MPU_cs_lock(mpu);
		temp[0] = MPU6050_ACCEL_CONFIG ;
		temp[1] = (temp[1] & 0xE7) | (uint8_t)acc_range << 3;
		mpu->hal_state = HAL_SPI_Transmit(mpu->hspi, temp,  2, 2);
		MPU_cs_unlock(mpu);
	}
	if(mpu->hal_state == HAL_OK){ // on vérifi si on a bien configuré le gyro comme on voulait
		uint8_t temp2 = temp[1];
		temp[0] = MPU6050_ACCEL_CONFIG | MPU6050_READ ;
		HAL_Delay(1);
		MPU_cs_lock(mpu);
		mpu->hal_state = HAL_SPI_TransmitReceive(mpu->hspi, temp, temp, 2, 2);
		MPU_cs_unlock(mpu);

		if(temp[1] != temp2)
			return SENSOR_NOT_INIT ;
	}

	#endif
#endif

	switch(acc_range){
		case MPU_ACC_2G :
			mpu->acc_sensi = (float)1 / MPU6050_ACCE_SENS_2 ;
			break;
		case MPU_ACC_4G :
			mpu->acc_sensi = (float)1 / MPU6050_ACCE_SENS_4 ;
			break;
		case MPU_ACC_8G :
			mpu->acc_sensi = (float)1 / MPU6050_ACCE_SENS_8 ;
			break;
		case MPU_ACC_16G :
			mpu->acc_sensi = (float)1 / MPU6050_ACCE_SENS_16 ;
			break;
	}
	return SENSOR_IDDLE;
}

/*
 *
 */
sensor_state_e MPU_update_gyro(mpu_t * mpu){

	//On réserve le mpu
	if(mpu->state != SENSOR_IDDLE){
		return SENSOR_ERROR ;
	}

	//Update des valeurs
#ifdef MPU_USE_I2C	//Lecture en i2C
	mpu->hal_state = HAL_I2C_Mem_Read(mpu->hi2c, mpu->adresse, MPU6050_GYRO_XOUT_H, I2C_MEMADD_SIZE_8BIT, mpu->gyro_data, 6, 5);
#else
	#ifdef MPU_USE_SPI	//Lecture en SPI
		uint8_t registers [] = {MPU6050_GYRO_XOUT_H | MPU6050_READ, 0, 0, 0, 0, 0};
		MPU_cs_lock(mpu);
		mpu->hal_state = HAL_SPI_TransmitReceive(mpu->hspi, registers, mpu->gyro_data, 6, 2);
		MPU_cs_unlock(mpu);
	#else
		return SENSOR_ERROR ;
	#endif
#endif


	if(mpu->hal_state != HAL_OK){
		return SENSOR_ERROR ;
	}

	//Convertion des valeurs
	convert_gyro(mpu);
	return SENSOR_IDDLE ;

}

sensor_state_e MPU_update_acc(mpu_t * mpu){

	//On réserve le mpu
	if(mpu->state != SENSOR_IDDLE){
		return SENSOR_ERROR ;
	}

	//Update des valeurs
#ifdef MPU_USE_I2C	//Lecture en i2C
	mpu->hal_state = HAL_I2C_Mem_Read(mpu->hi2c, mpu->adresse, MPU6050_GYRO_XOUT_H, I2C_MEMADD_SIZE_8BIT, mpu->gyro_data, 6, 5);
#else
	#ifdef MPU_USE_SPI	//Lecture en SPI
		uint8_t registers [] = {MPU6050_ACCEL_XOUT_H | MPU6050_READ, 0, 0, 0, 0, 0};
		MPU_cs_lock(mpu);
		mpu->hal_state = HAL_SPI_TransmitReceive(mpu->hspi, registers, mpu->acc_data, 6, 2);
		MPU_cs_unlock(mpu);
	#else
		return SENSOR_ERROR ;
	#endif
#endif

	if(mpu->hal_state != HAL_OK){
		return SENSOR_ERROR ;
	}

	//Convertion des valeurs
	convert_acc(mpu);
	return SENSOR_IDDLE ;
}

//Gestion du gyro en dma uniquement en I2C pour le moment (dma inutile quand le spi tourne à 10MHz .. )
sensor_state_e MPU_update_gyro_dma(mpu_t * mpu){

	mpu->start_time_dma = TIME_us();


	mpu->hal_state = HAL_I2C_Mem_Read_DMA(mpu->hi2c, mpu->adresse, MPU6050_GYRO_XOUT_H, I2C_MEMADD_SIZE_8BIT, mpu->gyro_data, 6);
	mpu->dma_state = MPU_DMA_GYRO_IN_PROGRESS ;

	if(mpu->hal_state == HAL_OK)
		mpu->state = SENSOR_IN_PROGRESS ;
	else if(mpu->hal_state == HAL_BUSY)
		mpu->state = SENSOR_HAL_BUSY ;
	else
		mpu->state = SENSOR_ERROR ;

	return mpu->state ;
}
void MPU_dma_transmit_done(mpu_t * mpu){
	if(!mpu->dma_state)
		return ;	//Si pas de lecture dma en cour on ignore

	else if(mpu->dma_state == MPU_DMA_GYRO_IN_PROGRESS)
		convert_gyro(mpu);
	else if(mpu->dma_state == MPU_DMA_ACC_IN_PROGREE)
		convert_acc(mpu);
	mpu->end_time_dma = TIME_us();
	mpu->delta_time_dma = mpu->end_time_dma - mpu->start_time_dma ;
	mpu->dma_state = MPU_DMA_IDDLE ;

}
























