#pragma once

#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"

//adress in flash to keep SystemParams_shell
#define SP_SHELL_FLASH_ADRESS 0x080FF800
//define this if your want update sturct wich were pushed to init_SystemParams_type when get new one in uart
#define UPDATE_USER_SP_BY_INTERRUPT

#include "SystemParams_tag.h"
//define data_size to optimize slip_uart
#define SLIP_UART_MAX_DATA_SIZE SystemParams_shell_size + 1
#include "slip_uart.h"

//consts
#define KEY_1 255
#define KEY_2 0
#define KEY_OK 255
#define KEY_ER 0

//------------------------------------------------------------------------------------------------------------------
error_status init_SystemParams_type(SystemParams_type* user_SP_pointer);//initializes SP from defined flash adress

//------------------------------------------------------------------------------------------------------------------
void configurate_SystemParams_trans(void);//needed to push input_handler to slip_uart and configurate slip_uart

//------------------------------------------------------------------------------------------------------------------
void input_handler(void);//work when get END_BYTE (full package) in input

//------------------------------------------------------------------------------------------------------------------
error_status write_SP_type_to_flash(const SystemParams_type* user_SP_pointer);//write SP to defined flash adress

//------------------------------------------------------------------------------------------------------------------
void clear_flash(uint32_t clear_addr);

error_status write_data_to_flash(uint32_t write_addr, const uint8_t* data, uint32_t data_size);

void read_data_from_flash(uint32_t read_addr, uint8_t* data, const uint32_t data_size);

//------------------------------------------------------------------------------------------------------------------
uint8_t crc8(const uint8_t* data, uint32_t data_size);
