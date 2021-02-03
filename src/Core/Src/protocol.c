#include "protocol.h"
#include "tim.h"
#include "adc.h"
#include "stm32f4xx_it.h"
#include "procedure.h"
#include "sound_alarm.h"
#include "configuration.h"
#include <stdbool.h>

uint32_t threshold;		// it is a threshold calculated by the ADC to understand the state of the photo resistor
extern system_status_t system_status;	// system_status structure defined in procedure file
extern system_config_t system_config;	// system_config structure defined in configuration file

bool pir_timer = false;   // it is used to understand if the timer delay of the pir is busy or not
bool barrier_timer = false; //it is used to understand if the timer delay of the barrier is busy or not

/*
 * It is the callback of the pir. when the signal is high, the pir timer is not busy and the area alarm is
 * active, the delay timer starts to check how long the signal remains high.
 * when the signal goes low or the area alarm goes low and the delay timer is busy, stop the timer that
 * counts the delay of the pir
*/

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == GPIO_PIN_3) {

		if (system_status.area_active && HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3) == 1
				&& !pir_timer) {
			//  SETTARE Init del timer e clear del timer
			MX_TIM9_Init();
			if(system_config.delay_pir!=0){
				__HAL_TIM_SET_AUTORELOAD(&htim9, (system_config.delay_pir * 1000) - 1);
			}
			HAL_TIM_Base_Start_IT(&htim9);
			pir_timer = true;
		}
		else if ((!system_status.area_active || HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3) == 0)
				&& pir_timer) {
			pir_timer = false;
			HAL_TIM_Base_Stop_IT(&htim9);
		}
	}
}

/*
 * It is the photo resistor callback, when the threshold value exceeds 1500 and the barrier alarm is active
 * and the delay timer is not busy, the timer starts to count the time in which the value remains above 1500.
 * when the threshold value falls below 1500 or the barrier alarm is deactivated and the delay timer is busy,
 * the timer is stopped.
 */

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
	// check the threshold
	if ((int) threshold > MAX_THRESHOLD && system_status.barrier_active
			&& !barrier_timer) {

		MX_TIM5_Init();
		if (system_config.delay_barrier != 0){
			__HAL_TIM_SET_AUTORELOAD(&htim5, (system_config.delay_barrier * 1000) - 1);
		}

		HAL_TIM_Base_Start_IT(&htim5);
		barrier_timer = true;

	} else if (((int) threshold < MAX_THRESHOLD || !system_status.barrier_active)
			&& barrier_timer) {
		barrier_timer = false;
		HAL_TIM_Base_Stop_IT(&htim5);
	}
}
