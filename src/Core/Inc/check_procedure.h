#ifndef INC_CHECK_PROCEDURE_H_
#define INC_CHECK_PROCEDURE_H_

#include <stdbool.h>

void command_rejected(char *);
void command_accepted(char *);
void user_pin_rejected(char *);
bool matchPin(char* pin, char* pin2);

#endif /* INC_CHECK_PROCEDURE_H_ */
