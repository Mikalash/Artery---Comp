#include "firmware_control.h"

_Bool is_firmware_valid(void)
{
	uint32_t crc = calc_firmware_crc();
	uint32_t crc_status = *((uint32_t*)FW_CRC_STATUS_FLASH_ADRESS);
	uint32_t crc_recorded = *((uint32_t*)FW_CRC_FLASH_ADRESS);
	
	if (crc_status != FW_CRC_RECORDED_STATUS)
	{
		if (record_firmware_crc_to_flash(crc) == SUCCESS)
			return TRUE;
		return FALSE;
	}
	
	if (crc_recorded ==  crc)
		return TRUE;
	return FALSE;
}

uint32_t calc_firmware_crc(void)
{
	crm_periph_clock_enable(CRM_CRC_PERIPH_CLOCK, TRUE);
	
	crc_data_reset();
	return crc_block_calculate((uint32_t*) FIRMWARE_FLASH_ADRESS, FIRMWARE_SIZE);
}

error_status record_firmware_crc_to_flash(const uint32_t crc)
{
	error_status er_status = SUCCESS;

	flash_unlock();
	flash_sector_erase(FW_CRC_STATUS_FLASH_ADRESS);
	
	er_status &= flash_word_program(FW_CRC_STATUS_FLASH_ADRESS, FW_CRC_RECORDED_STATUS);
	er_status &= flash_word_program(FW_CRC_FLASH_ADRESS, crc);
	
	if (er_status == ERROR)
		flash_sector_erase(FW_CRC_STATUS_FLASH_ADRESS);
	
	flash_lock();
	return er_status;
}

error_status clear_firmware_crc_from_flash(void)
{
	error_status er_status = SUCCESS;
	
	flash_unlock();
	flash_sector_erase(FW_CRC_STATUS_FLASH_ADRESS);
	flash_lock();
	
	return er_status;
}
