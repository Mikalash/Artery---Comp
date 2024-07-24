#include "settings_trans.h"

//------------------------------------------------------------------------------------------------------------------
error_status convert_sett_to_data(const struct settings_str* sett, uint8_t* data, const uint32_t data_size)
{
	uint32_t i;
	uint8_t* sett_address = (uint8_t*) sett;

	if (sett == NULL || data == NULL)
		return ERROR;
	if (data_size != sett_size)
		return ERROR;
		
	for (i = 0; i < data_size; i++)
		data[i] = sett_address[i];
	return SUCCESS;
}

error_status convert_data_to_sett(struct settings_str* sett, const uint8_t* data, const uint32_t data_size)
{
	uint32_t i;
	uint8_t* sett_address = (uint8_t*) sett;
	
	if (sett == NULL || data == NULL)
		return ERROR;
	if (data_size != sett_size)
		return ERROR;
	
	for (i = 0; i < data_size; i++)
		sett_address[i] = data[i];
	return SUCCESS;
}
//------------------------------------------------------------------------------------------------------------------
error_status write_sett_to_flash(const struct settings_str* sett)
{
	error_status result = ERROR;
	uint8_t crc_val = 0;
	uint8_t data[sett_size];
	
	if (sett == NULL)
		return ERROR;
	
	convert_sett_to_data(sett, data, sett_size);
	crc_val = crc8(data, sett_size);
	clear_flash(FLASH_ADRESS);
	result = write_data_to_flash(FLASH_ADRESS, data, sett_size);
	result &= write_data_to_flash(FLASH_ADRESS + sett_size, &crc_val, 1);
	return result;
}

void read_sett_from_flash(struct settings_str* sett)
{
	uint8_t data[sett_size];
	
	if (sett == NULL)
		return;
	
	read_data_from_flash(FLASH_ADRESS, data, sett_size);
	convert_sett_to_data(sett, data, sett_size);
}
//------------------------------------------------------------------------------------------------------------------
void clear_flash(uint32_t clear_addr)
{
	flash_unlock();
	flash_sector_erase(clear_addr);
	flash_lock();
}

error_status write_data_to_flash(uint32_t write_addr, const uint8_t* data, uint32_t data_size)
{
	uint32_t i;
	flash_status_type status = FLASH_OPERATE_DONE;
	
	if (data == NULL)
		return ERROR;
	
	flash_unlock();
	for(i = 0; i < data_size; i++)
	{
		status = flash_byte_program(write_addr, data[i]);
		if(status != FLASH_OPERATE_DONE)
		{
			flash_lock();
			return ERROR;
		}
		write_addr++;
	}
	flash_lock();
	return SUCCESS;
}

void read_data_from_flash(uint32_t read_addr, uint8_t* data, const uint32_t data_size)
{
	uint32_t i;
	
	if (data == NULL)
		return;
	
	for(i = 0; i < data_size; i++)
	{
		data[i] = *(uint8_t*)(read_addr);
		read_addr++;
	}
}
//------------------------------------------------------------------------------------------------------------------
uint8_t crc8(const uint8_t* data, uint32_t data_size)
{
	//crc_data_reset(); //crc integrated in artery, to use it, uncommit crc_configuration() in configurate_settings_trans()
	//return crc_block_calculate((uint32_t*) data_save, data_size_save);
	
	uint8_t crc = 0xFF;
	uint32_t i;
	
	if (data == NULL)
		return 0;
	
	while (data_size--)
	{
		crc ^= *data++;
		for (i = 0; i < 8; i++)
			crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;
	}
	return crc;
}

void crc_configuration(void)
{
	//enable crc clock
	crm_periph_clock_enable(CRM_CRC_PERIPH_CLOCK, TRUE);
	
	crc_poly_size_set(CRC_POLY_SIZE_8B);
	crc_init_data_set(0xFF);
	crc_poly_value_set(0x31);
}
