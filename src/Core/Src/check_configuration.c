#include <ctype.h>
#include "check_configuration.h"
#include "usart.h"
#include <stdbool.h>
#include "configuration.h"
#include <stdio.h>
#include <string.h>

extern uint8_t user_pin[USER_PIN]; // it is used to read the pin user chosen during the initial configuration
extern uint8_t buffer[BUFFER_SIZE]; // it is used to read all data (except the pin user) chosen during the initial configuration

/*
* @fn           int date, int month, int year
* @brief        check if the data inserted by the user is valid
* @param[in]    uint8_t months[12] it is used to read the number of days for each month
* @return       true if the data is valid, false otherwise
*/

bool check_date(int date, int month, int year) {
	uint8_t months[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	if (month < 0 || month > 12) {
		return false;
	}

	if (date < 0 || date > 31) {
		return false;
	}

	// data is valid if year > 2020
	if (year < 20) {
		return false;
	}

	if (year % 4 == 0 && month == 2) {
		if (date <= months[month - 1] + 1) {
			return true;
		}
	}

	if (date <= months[month - 1]) {
		return true;
	}

	return false;
}

/*
* @fn           int seconds, int minutes, int hours
* @brief        check if the time inserted by the user is valid
* @param[in]    seconds, minutes, hours
* @return       true if the time is valid, false otherwise
*/

bool check_time(int seconds, int minutes, int hours) {

	if (seconds >= 0 && seconds <= 59 && minutes >= 0 && minutes <= 59
			&& hours >= 0 && hours <= 23) {
		return true;
	} else {
		return false;
	}

}

/*
* @fn           int alarm_delay
* @brief        check if the alarm delay for each sensor is valid
* @param[in]    alarm_delay
* @return       true if the alarm delay for each sensor is valid, false otherwise
*/

bool check_alarm_delay(int alarm_delay) {

	if (alarm_delay >= 0 && alarm_delay <= 30) {
		return true;
	} else {
		return false;
	}
}

/*
* @fn           int alarm_duration
* @brief        check if the alarm duration for each sensor is valid
* @param[in]    alarm_duration
* @return       true if the alarm duration for each sensor is valid, false otherwise
*/

bool check_alarm_duration(int alarm_duration) {

	if (alarm_duration >= 1 && alarm_duration <= 60) {
		return true;
	} else {
		return false;
	}
}


/*
* @fn           no param
* @brief        check if the data inserted by the user (except the pin) are numbers
* @param[in]    buffer
* @return       true if the data in the buffer are number, false otherwise
*/

bool is_number() {

	for (int i = 0; i < BUFFER_SIZE; i++) {
		if (isdigit((char)buffer[i]) == 0) {
			return false;
		}
	}
	return true;
}


/*
* @fn           no param
* @brief        check if the user pin entered by the user has 4 digits
* @param[in]    USER_PIN is the dimension of the pin and user_pin is the buffer where the pin is stored
* @return       true if the pin is correct, false otherwise
*/

bool check_user_pin() {

	for (int i = 0; i < USER_PIN; i++) {
		if (isdigit((char)user_pin[i]) == 0) {
			return false;
		}
	}
	return true;
}

/*
* @fn           char *msg
* @brief        print a message on putty indicating rejected data entered
* @param[in]    msg is the message to print on putty
* @return       void function
*/

void configuration_rejected(char *msg) {
	HAL_UART_Transmit_DMA(&huart2, (uint8_t*) msg, strlen(msg));
}

/*
* @fn           char *msg
* @brief        print a message on putty indicating accepted data entered
* @param[in]    msg is the message to print on putty
* @return       void function
*/

void configuration_accepted(char *msg){
	HAL_UART_Transmit_DMA(&huart2, (uint8_t*) msg, strlen(msg));
}
