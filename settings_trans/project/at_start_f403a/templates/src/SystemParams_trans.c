#include "SystemParams_trans.h"

//------------------------------------------------------------------------------------------------------------------
#ifdef UPDATE_USER_SP_BY_INTERRUPT
	static volatile SystemParams_type* SP_init_pointer = NULL;
#endif

//------------------------------------------------------------------------------------------------------------------
error_status init_SystemParams_type(SystemParams_type* user_SP_pointer)
{
	SystemParams_shell* SP_shell = (SystemParams_shell*) SP_SHELL_FLASH_ADRESS;
	uint8_t crc_val = 0;

	if (user_SP_pointer == NULL || !is_firmware_valid())
		return ERROR;
	
	crc_val = crc8((uint8_t*) &SP_shell->SystemParams, SP_type_size);
	if (crc_val != SP_shell->SystemParams_crc)
		return ERROR;
	
	*user_SP_pointer = SP_shell->SystemParams;
	#ifdef UPDATE_USER_SP_BY_INTERRUPT
		SP_init_pointer = user_SP_pointer;
	#endif
	return SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------
void configurate_SystemParams_trans(void)
{
	configurate_slip_uart(input_handler);
}

//------------------------------------------------------------------------------------------------------------------
void input_handler(void)
{
	uint8_t crc_val = 0;
	
	uint8_t data[SP_shell_size + 1];
	uint32_t data_size = SP_shell_size + 1;
	
	data_size = read_slip_uart(data, data_size);
	
	if (data_size == SP_shell_size + 1 && data[0] == KEY_1)//read settings
	{
		SystemParams_shell* SP_shell = (SystemParams_shell*) &data[1];
		crc_val = crc8((uint8_t*) &SP_shell->SystemParams, SP_type_size);
		if (crc_val == SP_shell->SystemParams_crc)
		{
			clear_flash(SP_SHELL_FLASH_ADRESS);
			if (write_data_to_flash(SP_SHELL_FLASH_ADRESS, (uint8_t*) SP_shell, SP_shell_size) == SUCCESS)
			{
				#ifdef UPDATE_USER_SP_BY_INTERRUPT
					if (SP_init_pointer != NULL)
						*SP_init_pointer = SP_shell->SystemParams;
				#endif
				data[0] = KEY_OK;
			}
			else
				data[0] = KEY_ER;
		}
		else
			data[0] = KEY_ER;
		write_slip_uart(data, 1);
		return;
	}
	
	if (data_size == 1 && data[0] == KEY_2)//write settings
	{
		read_data_from_flash(SP_SHELL_FLASH_ADRESS, data, SP_shell_size);
		write_slip_uart(data, SP_shell_size); 
		return;
	}

	data[0] = KEY_ER;
	write_slip_uart(data, 1);
}

//------------------------------------------------------------------------------------------------------------------
error_status write_SP_type_to_flash(const SystemParams_type* user_SP_pointer)
{
	SystemParams_shell SP_shell;
	
	if (user_SP_pointer == NULL)
		return ERROR;
	
	SP_shell.SystemParams = *user_SP_pointer;
	SP_shell.SystemParams_crc = crc8((uint8_t*) user_SP_pointer, SP_type_size);
	
	clear_flash(SP_SHELL_FLASH_ADRESS);
	return write_data_to_flash(SP_SHELL_FLASH_ADRESS, (uint8_t*) &SP_shell, SP_shell_size);
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
