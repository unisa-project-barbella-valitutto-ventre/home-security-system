#include "sound_alarm.h"
#include "tim.h"
#include "configuration.h"
#include "procedure.h"
#include "adc.h"

extern system_status_t system_status;  // system_status structure defined in procedure file
extern system_config_t system_config; // system_status structure defined in configuration file

/*
 * This function checks the status of the alarms and calls the function by passing an enum
 */

void check_alarms() {

	if (system_status.area_alarmed && system_status.barrier_alarmed) {
		ring_alarm(AREA_BARRIER_ALARMED);
	}
	else if (system_status.area_alarmed && !system_status.barrier_alarmed) {
		ring_alarm(AREA_ALARMED);
	}
	else if (!system_status.area_alarmed && system_status.barrier_alarmed) {
		ring_alarm(BARRIER_ALARMED);
	}
}

/*
 * This function set the prescaler and the period to change the frequency and it set the duration of the alarm
 */

void ring_alarm(TState state) {
	MX_TIM3_Init();
	MX_TIM4_Init();

	switch (state) {

	case AREA_BARRIER_ALARMED:
		__HAL_TIM_SET_PRESCALER(&htim3, 2999);
		__HAL_TIM_SET_AUTORELOAD(&htim3, 99);
		__HAL_TIM_SET_AUTORELOAD(&htim4, (system_config.alarm_duration * 1000) - 1);
		HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_1);
		HAL_TIM_Base_Start_IT(&htim4);
		break;
	case AREA_ALARMED:
		__HAL_TIM_SET_PRESCALER(&htim3, 10);
		__HAL_TIM_SET_AUTORELOAD(&htim3, 999);
		__HAL_TIM_SET_AUTORELOAD(&htim4, (system_config.alarm_duration * 1000) - 1);
		HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_1);
		HAL_TIM_Base_Start_IT(&htim4);
		break;
	case BARRIER_ALARMED:
		__HAL_TIM_SET_PRESCALER(&htim3, 499);
		__HAL_TIM_SET_AUTORELOAD(&htim3, 99);
		__HAL_TIM_SET_AUTORELOAD(&htim4, (system_config.alarm_duration * 1000) - 1);
		HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_1);
		HAL_TIM_Base_Start_IT(&htim4);
		break;
	}
}

