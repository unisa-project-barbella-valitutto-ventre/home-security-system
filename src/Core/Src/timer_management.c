#include "timer_management.h"
#include "tim.h"
#include "configuration.h"
#include "system_log.h"
#include "usart.h"
#include "procedure.h"
#include "sound_alarm.h"
#include "protocol.h"
#include "sound_alarm.h"
#include "adc.h"
#include "ds1307rtc.h"

extern uint32_t threshold;  // threshold value defined in protocol file
extern system_status_t system_status;	// system_status structure defined in procedure file
extern bool pir_timer;		// pir_timer defined in protocol file
extern bool barrier_timer;	// barrier_timer defined in protocol file

uint16_t dutyCycle = 0;
uint8_t increase = 0;
uint16_t count_pwm = 0;

/*
 * TIM3 is used in PWM mode, This allows us to manipulate the behavior of output voltage of a Pin.
 * The timer output is linked to Buzzer Alarm (Channel 1), causing a different sounds for each situation.
 *
 * TIM4 counts the duration of the alarms, this is provided by the user when the system boots,
 * otherwise the timer uses the duration of the default configuration as the duration.
 *
 * TIM5 counts the delay of the Barrier Alarm, i.e. specifies how long the photo-resistor
 * value is greater than the threshold value for the alarm to sound.
 *
 * TIM9 counts the delay of the Area Alarm, i.e. specifies how long the signal
 * must remain high for the alarm to sound.
 *
 * When system boots TIM10 counts for 30 seconds waiting for the initial configuration. After this
 * if this occurs before 30 seconds, system starts and the timer starts counting ten seconds that is log interval.
 *
 * TIM11 is used to toggle user's Pin every half second when the system is active, and at the same time
 * when both system and Barrier Alarm are active it allows to sample the ADC signal.
 */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM10) {
		if (__HAL_TIM_GET_AUTORELOAD(&htim10) == 29999) {
			HAL_TIM_Base_Stop_IT(&htim10);
			HAL_UART_AbortReceive_IT(&huart2);
			default_configuration();
		}
		if (__HAL_TIM_GET_AUTORELOAD(&htim10) == 9999) {
			ds1307rtc_get_date_time();
		}
	}
	if (htim->Instance == TIM11) {
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
		if(!system_status.barrier_alarmed && system_status.barrier_active){
			HAL_ADC_Start_DMA(&hadc1, &threshold, 1);
		}
	}
	if (htim->Instance == TIM5) {
		//delay barrier
		if ((int) threshold > MAX_THRESHOLD && system_status.barrier_active) {
			system_status.barrier_alarmed = true;
			barrier_timer = false;
			check_alarms();
			HAL_ADC_Stop_DMA(&hadc1);
			HAL_TIM_Base_Stop_IT(&htim5);

		}
	}

	if (htim->Instance == TIM9) {
		if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3) == 1 && system_status.area_active) {
			system_status.area_alarmed = true;
			pir_timer = false;
			check_alarms();
			HAL_TIM_Base_Stop_IT(&htim9);
		}
	}

	if(htim->Instance == TIM4){
		HAL_TIM_PWM_Stop_IT(&htim3, TIM_CHANNEL_1);
		HAL_TIM_Base_Stop_IT(&htim4);

		if(system_status.area_alarmed && system_status.barrier_alarmed){
			system_status.area_alarmed = false;
			system_status.barrier_alarmed = false;
		}
		else if(system_status.area_alarmed && !system_status.barrier_alarmed){
			system_status.area_alarmed = false;
		}
		else if(!system_status.area_alarmed && system_status.barrier_alarmed){
			system_status.barrier_alarmed = false;
		}
	}
}

/*
 * This callback allows to module the alarm sounds
 */

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
	dutyCycle = HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_1);

	if (htim->Instance == TIM3) {
		count_pwm++;
		if (count_pwm % 1000) {
			if (dutyCycle >= __HAL_TIM_GET_AUTORELOAD(&htim3)) {
				increase = 0;
			}
			if (dutyCycle <= 0) {
				increase = 1;
			}

			if (increase) {
				dutyCycle += 1;
			} else {
				dutyCycle -= 1;
			}
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, dutyCycle);
		}

	}
}

