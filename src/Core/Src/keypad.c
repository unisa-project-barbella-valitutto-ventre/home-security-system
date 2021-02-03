#include "keypad.h"
#include "keyconfig.h"
#include "gpio.h"
#include <stdbool.h>

// We define a global variable in order to make the code simpler
#define _KEYPAD_DELAY(x)      HAL_Delay(x)

keypad_t KeyPad;
bool keypad_active = false; // it is used to understand when the keypad is active

//#############################################################################################

/*
 * Initializes the keyboard structure, including its size and
 * defining the structure of the GPIO Pins on the board.
 */

void KeyPad_Init(void) {
	GPIO_InitTypeDef gpio;
	KeyPad.ColumnSize = sizeof(_KEYPAD_COLUMN_GPIO_PIN) / 2;
	KeyPad.RowSize = sizeof(_KEYPAD_ROW_GPIO_PIN) / 2;
	for (uint8_t i = 0; i < KeyPad.ColumnSize; i++) {
		gpio.Mode = GPIO_MODE_OUTPUT_PP;
		gpio.Pull = GPIO_NOPULL;
		gpio.Speed = GPIO_SPEED_FREQ_LOW;
		gpio.Pin = _KEYPAD_COLUMN_GPIO_PIN[i];
		HAL_GPIO_Init((GPIO_TypeDef*) _KEYPAD_COLUMN_GPIO_PORT[i], &gpio);
		HAL_GPIO_WritePin((GPIO_TypeDef*) _KEYPAD_COLUMN_GPIO_PORT[i],
				_KEYPAD_COLUMN_GPIO_PIN[i], GPIO_PIN_SET);
	}
	for (uint8_t i = 0; i < KeyPad.RowSize; i++) {
		gpio.Mode = GPIO_MODE_INPUT;
		gpio.Pull = GPIO_PULLUP;
		gpio.Speed = GPIO_SPEED_FREQ_LOW;
		gpio.Pin = _KEYPAD_ROW_GPIO_PIN[i];
		HAL_GPIO_Init((GPIO_TypeDef*) _KEYPAD_ROW_GPIO_PORT[i], &gpio);
	}
	keypad_active = true;
}
//#############################################################################################
uint16_t KeyPad_Scan(void) {
	uint16_t key = 0;
	for (uint8_t c = 0; c < KeyPad.ColumnSize; c++) {
		for (uint8_t i = 0; i < KeyPad.ColumnSize; i++)
			HAL_GPIO_WritePin((GPIO_TypeDef*) _KEYPAD_COLUMN_GPIO_PORT[i],
					_KEYPAD_COLUMN_GPIO_PIN[i], GPIO_PIN_SET);
		HAL_GPIO_WritePin((GPIO_TypeDef*) _KEYPAD_COLUMN_GPIO_PORT[c],
				_KEYPAD_COLUMN_GPIO_PIN[c], GPIO_PIN_RESET);
		_KEYPAD_DELAY(5);
		for (uint8_t r = 0; r < KeyPad.RowSize; r++) {
			if (HAL_GPIO_ReadPin((GPIO_TypeDef*) _KEYPAD_ROW_GPIO_PORT[r],
					_KEYPAD_ROW_GPIO_PIN[r]) == GPIO_PIN_RESET) {
				_KEYPAD_DELAY(_KEYPAD_DEBOUNCE_TIME_MS);
				if (HAL_GPIO_ReadPin((GPIO_TypeDef*) _KEYPAD_ROW_GPIO_PORT[r],
						_KEYPAD_ROW_GPIO_PIN[r]) == GPIO_PIN_RESET) {
					key |= 1 << c;
					key |= 1 << (r + 8);
					while (HAL_GPIO_ReadPin((GPIO_TypeDef*) _KEYPAD_ROW_GPIO_PORT[r], 
						_KEYPAD_ROW_GPIO_PIN[r]) == GPIO_PIN_RESET)
						_KEYPAD_DELAY(5);
					return key;
				}
			}
		}
	}
	return key;
}
//#############################################################################################

/*
 * This function returns the hexadecimal value of the button pressed
 * on the keypad, scanning all rows and columns of it, searching for
 * the high value corresponding to the button
 */

uint16_t KeyPad_WaitForKey(uint32_t Timeout_ms) {
	uint16_t keyRead;
	while (Timeout_ms == 0) {
		keyRead = KeyPad_Scan();
		if (keyRead != 0) {
			KeyPad.LastKey = keyRead;
			return keyRead;
		}
		_KEYPAD_DELAY(_KEYPAD_DEBOUNCE_TIME_MS);
	}
	uint32_t StartTime = HAL_GetTick();
	while (HAL_GetTick() - StartTime < Timeout_ms) {
		keyRead = KeyPad_Scan();
		if (keyRead != 0) {
			KeyPad.LastKey = keyRead;
			return keyRead;
		}
		_KEYPAD_DELAY(_KEYPAD_DEBOUNCE_TIME_MS);
	}
	KeyPad.LastKey = 0;
	return 0;
}
//#############################################################################################

/*
* @fn           no param
* @brief        This function beeps whenever a key on the keypad is pressed
* @param[in]    PIN PB4
* @return       void function
*/

void buzz() {
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
	HAL_Delay(20);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
}
//#############################################################################################

/*
* @fn           uint32_t Timeout_ms is the maximum time that the function waits before returning the
* 				character pressed on the keypad
* @brief        This function calls KeyPad_WaitForKey(Timeout_ms)
* @return       the character pressed on the keypad
*/

char key_pad_get_char(uint32_t Timeout_ms) {
	switch (KeyPad_WaitForKey(Timeout_ms)) {
		case 0x0000:
			return 0;
		case 0x0101:
			buzz();
			return KEYPAD_VALUE_1;
		case 0x0201:
			buzz();
			return KEYPAD_VALUE_2;
		case 0x0401:
			buzz();
			return KEYPAD_VALUE_3;
		case 0x0801:
			buzz();
			return KEYPAD_VALUE_A;
		case 0x0102:
			buzz();
			return KEYPAD_VALUE_4;
		case 0x0202:
			buzz();
			return KEYPAD_VALUE_5;
		case 0x0402:
			buzz();
			return KEYPAD_VALUE_6;
		case 0x0802:
			buzz();
			return KEYPAD_VALUE_B;
		case 0x0104:
			buzz();
			return KEYPAD_VALUE_7;
		case 0x0204:
			buzz();
			return KEYPAD_VALUE_8;
		case 0x0404:
			buzz();
			return KEYPAD_VALUE_9;
		case 0x0804:
			buzz();
			return KEYPAD_VALUE_C;
		case 0x0108:
			buzz();
			return KEYPAD_VALUE_STAR;
		case 0x0208:
			buzz();
			return KEYPAD_VALUE_0;
		case 0x0408:
			buzz();
			return KEYPAD_VALUE_HASH;
		case 0x0808:
			buzz();
			return KEYPAD_VALUE_D;
		default:
			return KEYPAD_NO_PRESSED;
	}
}
//#############################################################################################

/*
* @fn           no param
* @brief        This function allows to obtain the entire pin entered by the user on the keypad
* @param[in]    bool is_num is used to check if the pin is a number, char val is the character pressed on the keypad
* 				char *new_pin is the pin typed by user
* @return       pin typed by user
*/

char* key_pad_get_pin() {
	bool is_num = false;
	int i = 0;
	char val, *new_pin = malloc(sizeof (char) * PIN_SIZE);
	do {
		val = key_pad_get_char(BUTTON_TIMEOUT);
		if (val != 0) {
			is_num = (val >= KEYPAD_VALUE_0 && val <= KEYPAD_VALUE_9);
			if (is_num) {
				new_pin[i] = val;
				i++;
			}
		}
	} while (i < PIN_SIZE);
	new_pin[4] = '\0';
	return new_pin;
}
