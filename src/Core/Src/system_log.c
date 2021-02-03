#include "ds1307rtc.h"
#include <stdbool.h>
#include "usart.h"
#include <string.h>
#include <stdio.h>
#include "procedure.h"

extern system_status_t system_status; // system_status structure defined in procedure file
extern date_time_t datetime;

char message[100];					  // buffer used to print on putty

/*
 * This function takes the date and time from the RTC and saves them in the datetime structure.
 * Returns a message in this format: 20-10-2020 11:15:47
 */
char* get_time() {

	sprintf(message, "[%hu-%hu-%hu %hu:%hu:%hu]", datetime.date, datetime.month,
			(uint16_t) (2000 + datetime.year), datetime.hours, datetime.minutes,
			datetime.seconds);
	return message;
}

/*
 * This function checks the system status, it takes date and time from the previous function and prints a
 * message on putty in this format: 20-10-2020 11:15:47 Area Disabled - Barrier Disabled
 */

void period_log() {

	char status_message[100];
	char final_message[100];
	char time_message[100];

	if (system_status.area_active && system_status.barrier_active) {
		if (system_status.area_alarmed && system_status.barrier_alarmed) {
			sprintf(status_message, "%s", " Area Alarmed – Barrier Alarmed");
		} else if (!system_status.area_alarmed
				&& system_status.barrier_alarmed) {
			sprintf(status_message, "%s", " Area Active – Barrier Alarmed");
		} else if (system_status.area_alarmed
				&& !system_status.barrier_alarmed) {
			sprintf(status_message, "%s", " Area Alarmed – Barrier Active");
		} else
			sprintf(status_message, "%s", " Area Active – Barrier Active");
	}

	if (system_status.area_active && !system_status.barrier_active) {
		if (system_status.area_alarmed) {
			sprintf(status_message, "%s", " Area Alarmed – Barrier Disabled");
		} else
			sprintf(status_message, "%s", " Area Active – Barrier Disabled");
	}

	if (!system_status.area_active && system_status.barrier_active) {
		if (system_status.barrier_alarmed) {
			sprintf(status_message, "%s", " Area Disabled – Barrier Alarmed");
		} else
			sprintf(status_message, "%s", " Area Disabled – Barrier Active");
	}

	if (!system_status.area_active && !system_status.barrier_active) {
		sprintf(status_message, "%s", " Area Disabled – Barrier Disabled");
	}

	sprintf(time_message, "%s", get_time());
	sprintf(final_message, "\r\n%s", strcat(time_message, status_message));
	HAL_UART_Transmit_DMA(&huart2, (uint8_t*) final_message,
			strlen(final_message));
}

