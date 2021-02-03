#include "check_procedure.h"
#include "usart.h"
#include <string.h>
#include <stdbool.h>

/*
* @fn           char *msg
* @brief        print a message on putty that indicates an invalid sequence
* @param[in]    msg
* @return       void function
*/

void command_rejected(char *msg) {
	HAL_UART_Transmit_DMA(&huart2, (uint8_t*) msg, strlen(msg));
}


/*
* @fn           char *msg
* @brief        print a message on putty that indicates a valid sequence
* @param[in]    msg
* @return       void function
*/

void command_accepted(char *msg) {
	HAL_UART_Transmit_DMA(&huart2, (uint8_t*) msg, strlen(msg));
}

/*
* @fn           char *msg
* @brief        print a message on putty indicating that the inserted pin is not valid
* @param[in]    msg
* @return       void function
*/

void user_pin_rejected(char *msg){
	HAL_UART_Transmit_DMA(&huart2, (uint8_t*) msg, strlen(msg));
}

/*
* @fn           char *pin, char *pin2
* @brief        check if pin and pin2 are equal or not
* @param[in]    pin, pin2
* @return       true if pin and pin2 are equal, false otherwise
*/

bool matchPin(char *pin, char *pin2) {
	if (strcmp(pin, pin2) == 0)
		return true;
	else
		return false;
}
