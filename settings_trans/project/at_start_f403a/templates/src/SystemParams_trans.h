#pragma once

#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"

//adress in flash to keep SystemParams_shell
#define SP_SHELL_FLASH_ADRESS 0x080FF800

//#include "SystemParams_tag.h"
//#include "rtwtypes.h"
#include "systemDefinations.h"

#pragma pack(push, 1)
typedef struct {
	SystemParams_st SystemParams;//124 bytes
	uint8_t SystemParams_crc;//4 bytes            //CRC of SystemParams
} SystemParams_shell;
#pragma pack(pop)

#define SP_type_size sizeof(SystemParams_st)
#define SP_shell_size sizeof(SystemParams_shell)
	
//define data_size to optimize slip_uart
#define SLIP_UART_MAX_DATA_SIZE SystemParams_shell_size + 1
#include "slip_uart.h"

//consts
#define KEY_1 255 //read settings from gui
#define KEY_2 0   //write settings from flash to gui
#define KEY_3 111 //write settings to flash
#define KEY_OK 255
#define KEY_ER 0

//------------------------------------------------------------------------------------------------------------------
error_status init_SystemParams_type(SystemParams_st* user_SP_pointer);//initializes SP from defined flash adress
//this pointer will use to store data from gui also

//------------------------------------------------------------------------------------------------------------------
void configurate_SystemParams_trans(void);//needed to push input_handler to slip_uart and configurate slip_uart

//------------------------------------------------------------------------------------------------------------------
void input_handler(void);//work when get END_BYTE (full package) in input

//------------------------------------------------------------------------------------------------------------------
error_status write_SP_type_to_flash(const SystemParams_st* user_SP_pointer);//write SP to defined flash adress

//------------------------------------------------------------------------------------------------------------------
void clear_flash(uint32_t clear_addr);

error_status write_data_to_flash(uint32_t write_addr, const uint8_t* data, uint32_t data_size);

void read_data_from_flash(uint32_t read_addr, uint8_t* data, const uint32_t data_size);

//------------------------------------------------------------------------------------------------------------------
uint8_t crc8(const uint8_t* data, uint32_t data_size);
