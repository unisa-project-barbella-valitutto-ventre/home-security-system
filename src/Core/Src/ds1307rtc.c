#include <stdint.h>
#include "i2c.h"
#include "ds1307rtc.h"
#include "system_log.h"

#define ADDRESS_SIZE				(1)
#define DATA_TRANSFER_SIZE			(7)
#define DATA_SIZE					(7)

#define DS1307_ADDRESS 				(0xD0)
#define MAX_RETRY					(3)

#define DS1307_SECONDS				(0x00)
#define DS1307_MINUTES				(0x01)
#define DS1307_HOURS				(0x02)
#define DS1307_DAY					(0x03)
#define DS1307_DATE					(0x04)
#define DS1307_MONTH				(0x05)
#define DS1307_YEAR					(0x06)
#define DS1307_CONTROL				(0x07)

/* Bits in control register */
#define DS1307_CONTROL_OUT			(7)
#define DS1307_CONTROL_SQWE			(4)
#define DS1307_CONTROL_RS1			(1)
#define DS1307_CONTROL_RS0			(0)

uint8_t out_buff[7];	// it is used to take the data in the rtc and to set these data in datetime structure in Receive Callback
date_time_t datetime;	// it is used to set the data and time, entered by the user during the initial configuration, in the RTC

/*
 * @fn          static uint8_t bcd2Dec ( uint8_t val )
 * @brief       Convert BCD to Decimal
 * @param[in]   val value to convert from BCD to Decimal
 * @return      res Converted value
 */
uint8_t bcd2Dec(uint8_t val) {
	uint8_t res = ((val / 16 * 10) + (val % 16));
	return res;
}

/*
 * @fn           static uint8_t dec2Bcd ( uint8_t val )
 * @brief        Convert Decimal to BCD
 * @param[in]    val value to convert from Decimal to BCD
 * @return       res Converted value
 */
uint8_t dec2Bcd(uint8_t val) {
	uint8_t res = ((val / 10 * 16) + (val % 10));
	return res;
}

int8_t ds1307rtc_init() {
	HAL_StatusTypeDef returnValue;
	returnValue = HAL_I2C_IsDeviceReady(&hi2c1, DS1307_ADDRESS, MAX_RETRY,
			HAL_MAX_DELAY);
	if (returnValue != HAL_OK) {
		return DS1307_ERR;
	}
	return DS1307_OK;
}

/*
 * @fn           date_time_t* datetime
 * @brief        it takes rtc data, converts them in decimal and saves them in datetime structure
 * @param[in]    out_buff is a buffer where the data are stored
 * @return       the operation status
 */

int8_t ds1307rtc_get_date_time() {
	HAL_StatusTypeDef returnValue;

	returnValue = HAL_I2C_Mem_Read_DMA(&hi2c1, DS1307_ADDRESS, DS1307_SECONDS,
			ADDRESS_SIZE, out_buff, DATA_SIZE);

	if (returnValue != HAL_OK) {
		return DS1307_IC2_ERR;
	}

	return DS1307_OK;
}

/*
 * @fn           const date_time_t* datetime
 * @brief        it takes datetime structure data, converts them in bcd and loads them in rtc
 * @param[in]    out_buff is a buffer where the data are stored
 * @return       the operation status
 */

int8_t ds1307rtc_set_date_time() {
	HAL_StatusTypeDef returnValue;
	uint8_t in_buff[7];

	in_buff[0] = dec2Bcd(datetime.seconds);
	in_buff[1] = dec2Bcd(datetime.minutes);
	in_buff[2] = dec2Bcd(datetime.hours);
	in_buff[3] = dec2Bcd(datetime.day);
	in_buff[4] = dec2Bcd(datetime.date);
	in_buff[5] = dec2Bcd(datetime.month);
	in_buff[6] = dec2Bcd(datetime.year);

	returnValue = HAL_I2C_Mem_Write(&hi2c1, DS1307_ADDRESS, DS1307_SECONDS,
	ADDRESS_SIZE, in_buff, DATA_TRANSFER_SIZE, HAL_MAX_DELAY);

	if (returnValue != HAL_OK) {
		return DS1307_IC2_ERR;
	}

	return DS1307_OK;

}

int8_t ds1307rtc_dispose() {
	return DS1307_OK;
}

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c){

}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c) {

	if (hi2c->Instance == I2C1) {

		datetime.seconds = bcd2Dec(out_buff[0]);
		datetime.minutes = bcd2Dec(out_buff[1]);
		datetime.hours = bcd2Dec(out_buff[2]);
		datetime.day = bcd2Dec(out_buff[3]);
		datetime.date = bcd2Dec(out_buff[4]);
		datetime.month = bcd2Dec(out_buff[5]);
		datetime.year = bcd2Dec(out_buff[6]);

		// Start log after receiving date and time from rtc
		period_log();
	}
}
