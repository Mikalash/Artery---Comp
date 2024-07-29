#pragma once

#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"

//configuration
#define FLASH_ADRESS 0x080FF800

//structure with settings and its size
struct settings_str
{
	int rpm;
	char direction;
	int temperature;
	float pressure;
};
//#define sett_size sizeof(struct settings_str)
#define sett_size 4//for debug
#define UART_MAX_DATA_SIZE sett_size + 2

/*
#include "SystemParams_tag.h"
#define UART_MAX_DATA_SIZE SystemParams_shell_size + 1
*/

#include "slip_uart.h"

//consts
#define KEY_1 255
#define KEY_2 0
#define KEY_OK 255
#define KEY_ER 0

static struct settings_str* settings = (struct settings_str*) FLASH_ADRESS;

//------------------------------------------------------------------------------------------------------------------
void input_handler(void);//work when get END_BYTE in input

//------------------------------------------------------------------------------------------------------------------
error_status convert_sett_to_data(const struct settings_str* sett, uint8_t* data, const uint32_t data_size);

error_status convert_data_to_sett(struct settings_str* sett, const uint8_t* data, const uint32_t data_size);
//------------------------------------------------------------------------------------------------------------------
error_status write_sett_to_flash(const struct settings_str* sett);

void read_sett_from_flash(struct settings_str* sett);
//------------------------------------------------------------------------------------------------------------------
void clear_flash(uint32_t clear_addr);

error_status write_data_to_flash(uint32_t write_addr, const uint8_t* data, uint32_t data_size);

void read_data_from_flash(uint32_t read_addr, uint8_t* data, const uint32_t data_size);
//------------------------------------------------------------------------------------------------------------------
uint8_t crc8(const uint8_t* data, uint32_t data_size);

//void crc_configuration(void);//crc integrated in artery
