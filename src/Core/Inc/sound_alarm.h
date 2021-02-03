/*
 * sound_alarm.h
 *
 *  Created on: 7 giu 2020
 *      Author: Andrew
 */

#ifndef INC_SOUND_ALARM_H_
#define INC_SOUND_ALARM_H_

typedef enum {
	AREA_ALARMED,
	BARRIER_ALARMED,
	AREA_BARRIER_ALARMED
} TState;

void ring_alarm(TState);
void check_alarms();

#endif /* INC_SOUND_ALARM_H_ */
