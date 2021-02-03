#ifndef	_KEYPAD_H
#define	_KEYPAD_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define NEWLINE 						"\n"
#define AREA_ALARM 						"Area Alarm! \n"
#define BARRIER_ALARM 					"Barrier Alarm! \n"
#define BOTH_ALARM 						"Area & Barrier Alarm! \n"
#define SYSTEM_ALARM 					"System Alarm! \n"
#define PIN_REQUIRED 					"Insert user pin: \n"

#define PIN_SIZE 						(4)
#define BUTTON_TIMEOUT					(1000)

#define KEYPAD_NO_PRESSED		 	    '\0'
#define KEYPAD_VALUE_1                  '1'
#define KEYPAD_VALUE_2                  '2'
#define KEYPAD_VALUE_3                  '3'
#define KEYPAD_VALUE_4                  '4'
#define KEYPAD_VALUE_5                  '5'
#define KEYPAD_VALUE_6                  '6'
#define KEYPAD_VALUE_7                  '7'
#define KEYPAD_VALUE_8                  '8'
#define KEYPAD_VALUE_9                  '9'
#define KEYPAD_VALUE_A                  'A'
#define KEYPAD_VALUE_B                  'B'
#define KEYPAD_VALUE_C                  'C'
#define KEYPAD_VALUE_D                  'D'
#define KEYPAD_VALUE_0                  '0'
#define KEYPAD_VALUE_HASH               '#'
#define KEYPAD_VALUE_STAR               '*'


/*
 * The following struct defines the keypad structure
 * composed by the size of rows and columns and keeping 
 * track of the last ket that has been pressed.
 */
typedef struct {
	uint8_t   ColumnSize;
	uint8_t   RowSize;
	uint16_t  LastKey;	
} keypad_t;

/*
 * Initializes the keyboard structure, inlcuding its size and 
 * defining the structure of the GPIO Pins on the board.
 */
void KeyPad_Init(void);

/*
 * This function returns the exadecimal value of the button pressed
 * on the keypad, scanning all rows and columns of it, searching for
 * the high value corresponding to the button
 */
uint16_t KeyPad_WaitForKey(uint32_t Timeout_ms);

/*
 * This function associates the exadecimal value returned by the above
 * function and turns them into characters
 */
char key_pad_get_char(uint32_t Timeout_ms);

/*
 * This function makes the buzzer sound for a little period of time
 * in order to make it sound each time a key is pressed
 */
void buzz(void);

/*
 * This function allows to get a 4 character pin and return it as a string
 */
char* key_pad_get_pin(void);

/*
 *
 */
char* KeyPad_GetInitialConfig(void);

/*
 *
 */
char* KeyPad_GetMode(void);

#endif
