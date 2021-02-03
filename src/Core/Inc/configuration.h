/*
 * configuration.h
 *
 *  Created on: Jun 3, 2020
 *      Author: Andrew
 */

#ifndef INC_CONFIGURATION_H_
#define INC_CONFIGURATION_H_

#include <stdbool.h>

#define USER_PIN (4)
#define BUFFER_SIZE (2)
#define VALUE_NOT_ASSIGNED (-1)

#define DATE_NOT_VALID "\n\rDate not valid!.... Repeat Please, insert day of month: "
#define TIME_NOT_VALID "\n\rTime not valid!.... Repeat Please, insert seconds: "
#define ALARM_DELAY_NOT_VALID "\n\rAlarm delay not valid!.... Repeat Please: "
#define ALARM_DURATION_NOT_VALID "\n\rAlarm duration not valid!.... Repeat Please: "
#define USER_PIN_ERROR "\n\rUser pin not valid... Repeat Please: "

/*
 * a representation of the system configuration
 */
struct system_config_s
{
	char 	pin[5];
	int     delay_pir;
	int     delay_barrier;
	int     alarm_duration;
};

typedef struct system_config_s system_config_t;

/*
 * 	if within 30 seconds when the system is turned on, the user does
 *	not enter anything, the system will have a default configuration
 */
void default_configuration(void);

/*
 * This function takes the user configuration within 30 seconds when the system is turned on
*/
void user_configuration(void);


#endif /* INC_CONFIGURATION_H_ */
