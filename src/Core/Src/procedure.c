#include "procedure.h"
#include "check_procedure.h"
#include "configuration.h"
#include "stm32f4xx_hal.h"
#include <ctype.h>
#include <string.h>
#include "keypad.h"
#include "tim.h"

extern system_config_t system_config;  // is the structure defined in the initial configuration
char abcd[3];		//is a buffer in which at index zero there is the letter
					// of the sequence and at index 1 the symbol # or * to activate or deactivate an alarm

system_status_t system_status; // it is a structure to store the system status

/*
* @fn           no param
* @brief        It starts when the user enter " # ", after this, it is required the user pin,
*				if it matches with the previous chosen during the configuration, it is required two other symbols
*				(a letter and  one between "#" and " * "). If the last two symbols are valid, it calls setOrReset function
* @param[in]    key_pad_get_char returns the character pressed on the keypad
* @return       void function
*/

void start_sequence() {
	char *pin_user;

	if (key_pad_get_char(200) == KEYPAD_VALUE_HASH) {
		command_accepted(COMMAND_ACCEPTED);
		pin_user = key_pad_get_pin();
		if (matchPin(pin_user, system_config.pin)) {
			/* WAITING FOR A B C D */
			getSymbols();
			if (checkSymbol()) {
				setOrReset();
			}
		}else
			user_pin_rejected(USER_PIN_REJECTED);
	}
}

/*
* @fn           no param
* @brief        it takes the last two symbols of the sequence inserted by the user calling key_pad_get_char function
* @param[in]    abcd buffer to store the last two symbols of the sequence
* @return       void function
*/

void getSymbols() {
	int i = 0;
	clear_vector();

	do {
		abcd[i] = key_pad_get_char(0);
		if (i == 0) {
			if (abcd[i] == '\0') {
				i = 0;
			} else {
				i++;
			}
		}
		if (i == 1) {
			if (abcd[i] == '\0') {
				i = 1;
			} else {
				i++;
				abcd[i] = '\0';
			}
		}
	} while (abcd[0] == '\0' || abcd[1] == '\0');

}

/*
* @fn           no param
* @brief        it checks if last two symbols of the sequence are valid or not
* @param[in]    abcd buffer
* @return       true if last two symbols are correct, false otherwise
*/

bool checkSymbol() {
	if ((abcd[1] != '#' && abcd[1] != '*')
			|| (abcd[0] != 'A' && abcd[0] != 'B'
					&& abcd[0] != 'C'
					&& abcd[0] != 'D')) {
		clear_vector();
		command_rejected(COMMAND_REJECTED);
		return false;
	}
	return true;
}

/*
* @fn           no param
* @brief        it cleans the vector at the end of the sequence in order to be reused
* @param[in]    abcd buffer
* @return       void function
*/

void clear_vector(void) {
	for (int i = 0; i < 2; i++) {
		abcd[i] = '\0';
	}
}


/*
* @fn           no param
* @brief        set the system status (all disable) when it is activated
* @param[in]    all attributes of system_status structure
* @return       void function
*/

void boot_system() {
	system_status.area_active = false;
	system_status.barrier_active = false;
	system_status.area_alarmed = false;
	system_status.barrier_alarmed = false;
	system_status.system_active = false;
}

/*
* @fn           no param
* @brief        set the system status according to the sequence entered by the user
* @param[in]    all attributes of system_status structure, abcd buffer and
* 				correct_sequence to indicate whether the sequence entered is valid or not
* @return       void function
*/

void setOrReset(void) {
	bool correct_sequence = false;

	if (system_status.system_active) {
		if (abcd[1] == '#') {
			switch (abcd[0]) {
			case 'A':
				if (system_status.area_active)
					correct_sequence = false;
				else {
					system_status.area_active = true;
					HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_SET);
					correct_sequence = true;
				}
				break;

			case 'B':
				if (system_status.barrier_active){
					correct_sequence = false;
				}else {
					system_status.barrier_active = true;
					// Turn on the laser
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
					correct_sequence = true;
				}
				break;

			case 'C':
				if (system_status.barrier_active && system_status.area_active)
					correct_sequence = false;
				else {
					system_status.area_active = true;
					system_status.barrier_active = true;
					// Turn on the laser
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
					correct_sequence = true;
				}
				break;
			}
		}
		if (abcd[1] == '*') {
			switch (abcd[0]) {
			case 'A':
				if (!system_status.area_active)
					correct_sequence = false;
				else {
					system_status.area_active = false;
					correct_sequence = true;
				}
				break;

			case 'B':
				if (!system_status.barrier_active)
					correct_sequence = false;
				else {
					system_status.barrier_active = false;
					// Turn off the laser
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
					correct_sequence = true;
				}
				break;

			case 'C':
				if (!system_status.barrier_active && !system_status.area_active)
					correct_sequence = false;
				else {
					system_status.area_active = false;
					system_status.barrier_active = false;
					// Turn off the laser
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
					correct_sequence = true;
				}
				break;
			}
		}
	}// Active system
	else if (abcd[0] == 'D' && abcd[1] == '#'
			&& !system_status.system_active) {
		system_status.system_active = true;
		// Start timer for toggle user led
		HAL_TIM_Base_Start_IT(&htim11);
		correct_sequence = true;
	} else if (abcd[0] == 'D' && abcd[1] == '#' && system_status.system_active)
		correct_sequence = false;
	else
		correct_sequence = false;
	//Disable system
	if (abcd[0] == 'D' && abcd[1] == '*' && system_status.system_active) {
		//Disable all sensors
		system_status.system_active = false;
		system_status.area_active = false;
		system_status.barrier_active = false;
		// Turn off the led
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
		// Stop timer for toggle user led
		HAL_TIM_Base_Stop_IT(&htim11);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
		correct_sequence = true;
	} else if (abcd[0] == 'D' && abcd[1] == '*' && !system_status.system_active)
		correct_sequence = false;

	if (correct_sequence) {
		// active a short sound to indicate that the sequence is valid
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
		HAL_Delay(499);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
		// print on putty a message to indicate a command accepted
		command_accepted(COMMAND_ACCEPTED);
	} else {
		// print on putty a message to indicate a command rejected because the sequence is not valid
		command_rejected(COMMAND_REJECTED);
	}

	clear_vector();
}
