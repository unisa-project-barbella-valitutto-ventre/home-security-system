
#ifndef INC_PROCEDURE_H_
#define INC_PROCEDURE_H_
#include <stdbool.h>
#define USER_PIN_REJECTED "\r\nUser Pin Rejected!"
#define COMMAND_REJECTED "\r\nCommand Rejected!"
#define COMMAND_ACCEPTED "\r\nCommand Accepted!"
#define SYSTEM_DEACTIVED "\r\nCommand Rejected, System is not active!"
#define ALARM_ALREADY_ACTIVED "\r\nCommand Rejected, Alarm already actived!"
#define ALARMS_ALREADY_ACTIVED "\r\nCommand Rejected, Alarms already actived!"
#define ALARM_ALREADY_DEACTIVED "\r\nCommand Rejected, Alarm already deactived!"
#define ALARMS_ALREADY_DEACTIVED "\r\nCommand Rejected, Alarms already deactived!"

/*
 * A representation of the system status
 */
struct system_status_s
{
	bool 	area_active;
	bool 	barrier_active;
	bool 	system_active;
	bool 	area_alarmed;
	bool	barrier_alarmed;
};

typedef struct system_status_s system_status_t;

void start_sequence();
void activeDeactive();
void setOrReset();
void getSymbols();
bool checkSymbol();
void clear_vector();
void boot_system();

#endif /* INC_PROCEDURE_H_ */
