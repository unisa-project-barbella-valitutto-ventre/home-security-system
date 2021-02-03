#ifndef	_KEYPADCONFIG_H
#define	_KEYPADCONFIG_H

#include "gpio.h"

#define _KEYPAD_DEBOUNCE_TIME_MS        (20)
#define _KEYPAD_USE_FREERTOS            (0)

/*
 * In this header file we define the pins of the keypad 
 * that are linked to the STM32
 */

const GPIO_TypeDef* _KEYPAD_COLUMN_GPIO_PORT[] = {
  GPIOC,
  GPIOB,
  GPIOB,
  GPIOB
};

const uint16_t _KEYPAD_COLUMN_GPIO_PIN[] = {
  GPIO_PIN_4,
  GPIO_PIN_13,
  GPIO_PIN_15,
  GPIO_PIN_2
};

const GPIO_TypeDef* _KEYPAD_ROW_GPIO_PORT[] = {
  GPIOB,
  GPIOA,
  GPIOC,
  GPIOC
};

const uint16_t _KEYPAD_ROW_GPIO_PIN[] = {
  GPIO_PIN_12,
  GPIO_PIN_12,
  GPIO_PIN_5,
  GPIO_PIN_8
};

#endif