#include "settings_trans.h"

error_status convert_sett_to_data(const struct settings_str* sett, uint8_t* data, const uint32_t data_size)
{
	uint32_t i;
	uint8_t* sett_address = (uint8_t*) sett;

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
	
	if (data_size != sett_size)
		return ERROR;
	
	for (i = 0; i < data_size; i++)
		sett_address[i] = data[i];
	return SUCCESS;
}
//------------------------------------------------------------------------------------------------------------------
error_status write_sett_to_flash(const struct settings_str* sett)
{
	uint8_t data[sett_size];
	convert_sett_to_data(sett, data, sett_size);
	return write_data_to_flash(FLASH_ADRESS, data, sett_size);
}

void read_sett_from_flash(struct settings_str* sett)
{
	uint8_t data[sett_size];
	read_data_from_flash(FLASH_ADRESS, data, sett_size);
	convert_sett_to_data(sett, data, sett_size);
}
//------------------------------------------------------------------------------------------------------------------
error_status write_data_to_flash(uint32_t write_addr, const uint8_t* data, uint32_t data_size)
{
	uint32_t i;
	flash_status_type status = FLASH_OPERATE_DONE;
	flash_unlock();
	
	flash_sector_erase(FLASH_ADRESS);
	
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
	for(i = 0; i < data_size; i++)
	{
		data[i] = *(uint8_t*)(read_addr);
		read_addr++;
	}
}
//------------------------------------------------------------------------------------------------------------------
void write_usart(const uint8_t* data, const uint32_t data_size)
{
	uint32_t i = 0;
	for (; i < data_size; i++)
	{
		while (usart_flag_get(USART1, USART_TDBE_FLAG) == RESET);
		usart_data_transmit(USART1, data[i]);
	}
}

void read_usart(uint8_t* data, const uint32_t data_size)
{
	uint32_t i = 0;
	for (; i < data_size; i++)
	{
		while (usart_flag_get(USART1, USART_RDBF_FLAG) == RESET);
		data[i] = usart_data_receive(USART1);;
	}
}
//------------------------------------------------------------------------------------------------------------------
void USART1_IRQHandler(void)
{
	uint8_t data[sett_size];
	uint8_t crc_val = 0;
	uint8_t lead_byte = LEAD_BYTE_DF;
	uint8_t end_byte = END_BYTE_DF;
	
	usart_interrupt_enable(USARTx, USART_RDBF_INT, FALSE);
	read_usart(&lead_byte, 1);
	
	if (lead_byte == LEAD_BYTE_RD)
	{
		read_usart(data, sett_size);
		read_usart(&crc_val, 1);;
		if (crc_val == crc8(data, sett_size))
		{
			//convert_data_to_sett(&settings, data, sett_size);
			//write_sett_to_flash(&settings);
			write_data_to_flash(FLASH_ADRESS, data, sett_size); //x2 faster
			end_byte = END_BYTE_CRC_OK;
		}
		else
			end_byte = END_BYTE_CRC_ER;
		write_usart(&end_byte, 1);
	}
	else if (lead_byte == LEAD_BYTE_WR)
	{
		//read_sett_from_flash(&settings);
		//convert_sett_to_data(&settings, data, sett_size);
		read_data_from_flash(FLASH_ADRESS, data, sett_size); //x2 faster
		crc_val = crc8(data, sett_size);
		write_usart(data, sett_size);
		write_usart(&crc_val, 1);
		//not necessarily
		//read_usart(&end_byte, 1);
	}
	
	usart_interrupt_enable(USARTx, USART_RDBF_INT, TRUE);
}
//------------------------------------------------------------------------------------------------------------------
uint8_t crc8(const uint8_t* data, uint32_t data_size)
{
	//crc_data_reset(); //crc integrated in artery, to use it, uncommit crc_configuration() in configurate_settings_trans()
	//return crc_block_calculate((uint32_t*) data_save, data_size_save);
	
	uint8_t crc = 0xFF;
	uint32_t i;
	while (data_size--)
	{
		crc ^= *data++;
		for (i = 0; i < 8; i++)
			crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;
	}
	return crc;
}
//------------------------------------------------------------------------------------------------------------------
void configurate_settings_trans(void)
{
	gpio_configuration();
	uart_configuration();
	//crc_configuration(); //i used own crc
}

void gpio_configuration(void)
{
	//gpio_conf
	gpio_init_type gpio_init_struct;
	//enable gpio clock
	crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);

	gpio_default_para_init(&gpio_init_struct);

	//configure the usart tx pin 
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
	gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
	gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
	gpio_init_struct.gpio_pins = USART_TX_PIN;
	gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
	gpio_init(GPIOA, &gpio_init_struct);

	//configure the usart rx pin
	gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
	gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
	gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
	gpio_init_struct.gpio_pins = USART_RX_PIN;
	gpio_init_struct.gpio_pull = GPIO_PULL_UP;
	gpio_init(GPIOA, &gpio_init_struct);
}

void uart_configuration(void)
{
	//enable usart clock
	crm_periph_clock_enable(CRM_USARTx_PERIPH_CLOCK, TRUE);

	usart_init(USARTx, USART_BOUD, USART_DATA_8BITS, USART_STOP_1_BIT);
	usart_transmitter_enable(USARTx, TRUE);
	usart_receiver_enable(USARTx, TRUE);
	
	usart_interrupt_enable(USARTx, USART_RDBF_INT, TRUE);
	usart_interrupt_enable(USARTx, USART_TDBE_INT, FALSE);
	usart_enable(USARTx, TRUE);
	
	nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
	nvic_irq_enable(USARTx_IRQn, 0, 1);
}

void crc_configuration(void)
{
	//enable crc clock
	crm_periph_clock_enable(CRM_CRC_PERIPH_CLOCK, TRUE);
	
	crc_poly_size_set(CRC_POLY_SIZE_8B);
	crc_init_data_set(0xFF);
	crc_poly_value_set(0x31);
}
