#include "configuration.h"
#include "ds1307rtc.h"
#include "usart.h"
#include <stdlib.h>
#include "ds1307rtc.h"
#include "tim.h"
#include <stdbool.h>
#include "check_configuration.h"
#include <stdio.h>
#include <string.h>
#include "procedure.h"
#include "keypad.h"

extern date_time_t datetime;	//datetime structure defined in ds1307rtc file

int count = 0; 			// it is used in HAL_UART_RxCpltCallback to print different messages on putty
int count_rec = 0;		// it is used in HAL_UART_TxCpltCallback to read different data on putty
char message[120];		// it is used in sprintf to copy the message to print on putty

uint8_t user_pin[USER_PIN];  // this buffer it is used to store the pin entered by the user on putty during the initial configuration
uint8_t buffer[BUFFER_SIZE]; // this buffer it is used to store the data entered by the user on putty (except the pin) during the initial configuration


system_config_t system_config;  // it is a structure to store the the pin user, area delay, barrier delay and alarm duration chosen during the configuration


/*
* @fn           no param
* @brief        set the attributes of the system_config structure to -1 before the configuration
* @param[in]    delay_pr, delay_barrier and alarm_duration are the attribute of the system_config structure
* @return       void function
*/

void setup(void){
	system_config.delay_pir = VALUE_NOT_ASSIGNED;
	system_config.delay_barrier = VALUE_NOT_ASSIGNED;
	system_config.alarm_duration = VALUE_NOT_ASSIGNED;
}

/*
* @fn           no param
* @brief        If the user doesn't provide any configuration, save the default configuration
* @param[in]    pin, delay_pir, delay_barrier, alarm_duration and date and time are set to default values
* @return       void function
*/

void default_configuration(void) {
	for (int i = 0; i < 4; i++) {
		system_config.pin[i] = '0';
	}
	system_config.pin[4] = '\0';

	system_config.delay_pir = 0;
	system_config.delay_barrier = 0;
	system_config.alarm_duration = 5;

	count_rec = 3;
	sprintf(message, "%s", "\n\rSystem Configuration Rejected! ---- Default Configuration Loaded! ----");
	HAL_UART_Transmit_DMA(&huart2, (uint8_t*) message, strlen(message));

}

/*
* @fn           no param
* @brief        start the communication with user through putty
* @param[in]    message param is the message to be printed on putty,
* @return       void function
*/

void user_configuration(void) {
	setup();
	sprintf(message,"%s","\n\r\n\r\n\r\n\r\n\r############### HOME SECURITY SYSTEM ###############\n\r-------- SYSTEM BOOT --------"
			"\n\rInsert User Pin [XXXX]: ");
	HAL_UART_Transmit_DMA(&huart2, (uint8_t*) message, strlen(message));
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle) {
	switch (count) {
	case 0:
		if (check_user_pin()) {
			count = count + 1;
			for (int i = 0; i < 4; i++) {
				system_config.pin[i] = user_pin[i];
			}
			system_config.pin[4] = '\0';
			count_rec = 1;
			configuration_accepted("\n\rUser Pin accepted! - Insert Pir Delay [max 30 sec]:");
		} else {
			configuration_rejected(USER_PIN_ERROR);
		}
		break;

	case 1:
		if (is_number()) {
			system_config.delay_pir = atoi((const char*) buffer);
			if (check_alarm_delay(system_config.delay_pir)) {
				count = count + 1;
				configuration_accepted("\n\rPir Delay accepted! - Insert Barrier Delay [max 30 sec]: ");
			} else {
				configuration_rejected(ALARM_DELAY_NOT_VALID);
			}
		} else {
			configuration_rejected(ALARM_DELAY_NOT_VALID);
		}
		break;

	case 2:
		if (is_number()) {
			system_config.delay_barrier = atoi((const char*) buffer);
			if (check_alarm_delay(system_config.delay_barrier)) {
				count = count + 1;
				configuration_accepted("\n\rBarrier Delay accepted! - Insert Alarms Duration [max 60 sec]: ");
			} else {
				configuration_rejected(ALARM_DELAY_NOT_VALID);
			}
		} else {
			configuration_rejected(ALARM_DELAY_NOT_VALID);
		}
		break;

	case 3:
		if (is_number()) {
			system_config.alarm_duration = atoi((const char*) buffer);
			if (check_alarm_duration(system_config.alarm_duration)) {
				count = count + 1;
				configuration_accepted("\n\rAlarm Duration accepted!\n\r---Date and Time---\n\rInsert Seconds:");
			} else {
				configuration_rejected(ALARM_DURATION_NOT_VALID);
			}
		} else {
			configuration_rejected(ALARM_DURATION_NOT_VALID);
		}
		break;

	case 4:
		if (is_number()) {
			datetime.seconds = (uint8_t) (atoi((const char*) buffer));
			count = count + 1;
			configuration_accepted("\n\rInsert Minutes: ");
		}
		break;
	case 5:
		if (is_number()) {
			datetime.minutes = (uint8_t) (atoi((const char*) buffer));
			count = count + 1;
			configuration_accepted("\n\rInsert Hours: ");
		}
		break;
	case 6:
		if (is_number()) {
			datetime.hours = (uint8_t) (atoi((const char*) buffer));
			if (check_time((int) datetime.seconds, (int) datetime.minutes,
					(int) datetime.hours)) {
				count = count + 1;
				configuration_accepted("\n\rTime accepted! - Insert the Day of Month: ");
			} else {
				count = 4;
				configuration_rejected(TIME_NOT_VALID);
			}
		}
		break;

	case 7:
		if (is_number()) {
			datetime.date = (uint8_t) (atoi((const char*) buffer));
			count = count + 1;
			configuration_accepted("\n\rDay of Month accepted! - Insert Month: ");
		}
		break;

	case 8:
		if (is_number()) {
			datetime.month = (uint8_t) (atoi((const char*) buffer));
			count = count + 1;
			configuration_accepted("\n\rMonth accepted! -  Insert Year: ");
		}
		break;

	case 9:
		if (is_number()) {
			datetime.year = (uint8_t) (atoi((const char*) buffer));
			if (check_date((int) datetime.date, (int) datetime.month,
					(int) datetime.year)) {
				ds1307rtc_set_date_time();
				count = count + 1;
				count_rec = 4;
				configuration_accepted("\n\r--------- System Configuration Loaded! ---------");
			} else {
				count = 7;
				configuration_rejected(DATE_NOT_VALID);
			}
		}
		break;
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle) {
	switch (count_rec) {

	case 0:
		HAL_UART_Receive_IT(&huart2, user_pin, USER_PIN);
		break;
	case 1:
		HAL_UART_Receive_IT(&huart2, buffer, BUFFER_SIZE);
		break;
	case 3: // Default configuration loaded
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
		KeyPad_Init();
		// Start TIM10 for system log
		MX_TIM10_Init();
		__HAL_TIM_SET_AUTORELOAD(&htim10,9999);
		HAL_TIM_Base_Start_IT(&htim10);
		// At first all the alarms are disabled
		boot_system();
		count_rec = 5;
		break;
	case 4: // User configuration loaded
		HAL_TIM_Base_Stop_IT(&htim10);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
		KeyPad_Init();
		// Start TIM10 for system log
		MX_TIM10_Init();
		__HAL_TIM_SET_AUTORELOAD(&htim10,9999);
		HAL_TIM_Base_Start_IT(&htim10);
		// At first all the alarms are disabled
		boot_system();
		count_rec = 5;
		break;

	}
}
