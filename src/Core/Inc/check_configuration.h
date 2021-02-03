/*
 * check_configuration.h
 *
 *  Created on: Jun 4, 2020
 *      Author: Andrew
 */

#ifndef INC_CHECK_CONFIGURATION_H_
#define INC_CHECK_CONFIGURATION_H_
#include <stdbool.h>



bool check_date(int , int , int);
bool check_time(int , int , int);
bool check_alarm_delay(int );
bool check_alarm_duration(int );
bool is_number();
bool check_user_pin();
void configuration_rejected(char*);
void configuration_accepted(char*);

#endif /* INC_CHECK_CONFIGURATION_H_ */
