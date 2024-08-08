#pragma once

#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"

//firmware parameters
#define FIRMWARE_FLASH_ADRESS 0x08000000
#define FIRMWARE_SIZE (0x1C50) / 4
//adress to keep crc
#define FW_CRC_STATUS_FLASH_ADRESS 0x08080000
#define FW_CRC_FLASH_ADRESS (FW_CRC_STATUS_FLASH_ADRESS + 4) 
//consts
#define FW_CRC_RECORDED_STATUS 0

_Bool is_firmware_valid(void);

uint32_t calc_firmware_crc(void);

error_status record_firmware_crc_to_flash(const uint32_t firmware_crc);

error_status clear_firmware_crc_from_flash(void);
