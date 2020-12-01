/*
 * sequence_led.h
 *
 *  Created on: 22 juin 2020
 *      Author: Th�o
 */

#ifndef LIB_PERSO_SEQUENCE_LED_H_
#define LIB_PERSO_SEQUENCE_LED_H_

#include "stm32f4xx_hal.h"
#include "../../Core/OS/macro_types.h"

#define SEQUENCE_LED_1 		0b101000101000
#define SEQUENCE_LED_2 		0b101010111100
#define SEQUENCE_LED_3 		0b101010101010
#define SEQUENCE_LED_4 		0b100010101000
#define SEQUENCE_LED_5 		0b101000000000
#define SEQUENCE_LED_6 		0b101010000000
#define SEQUENCE_LED_7 		0b101010100000
#define SEQUENCE_LED_8 		0b111111000000
#define SEQUENCE_LED_9		0b111111111111

//Structure utilis� par les fonctions s�quence led
typedef struct{
	GPIO_TypeDef* gpio;
	uint16_t gpio_pin ;
	uint32_t periode ;
	int32_t sequence ;		//Sequences lues de gauche � droite
	int8_t compteur ;
	int8_t length_sequence ;
	uint32_t previous_time ;
	bool_e output_logic ;	//True si logique n�gative au niveau de la led
}sequence_led_t;

void LED_SEQUENCE_init(sequence_led_t * seq_led, GPIO_TypeDef* gpio, uint16_t gpio_pin, int32_t init_seq, uint32_t periode, int8_t length_sequence, bool_e output_logic);
void LED_SEQUENCE_set_sequence(sequence_led_t * seq_led, int32_t seq);
void LED_SEQUENCE_play(sequence_led_t * seq_led, uint32_t current_time_us);

#endif /* LIB_PERSO_SEQUENCE_LED_H_ */
