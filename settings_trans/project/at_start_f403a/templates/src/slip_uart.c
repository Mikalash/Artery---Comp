#include "slip_uart.h"

//------------------------------------------------------------------------------------------------------------------
//buffer with stuffed byted and END_BYTE wich go to UART
static uint8_t uart_tx_buffer[UART_BUFFER_SIZE];
volatile static uint32_t uart_tx_counter = 0;

//buffer with stuffed byted and END_BYTE wich get from UART
static uint8_t uart_rx_buffer[UART_BUFFER_SIZE];
volatile static uint32_t uart_rx_counter = 0;
//true when get END_BYTE from UART
volatile static _Bool RX_BUFFER_READY = FALSE;
//------------------------------------------------------------------------------------------------------------------
void input_handler(void)
{
	//uint32_t i = 0;
	uint8_t crc_val = 0;
	
	uint8_t data[sett_size + 2];
	uint32_t data_size = sett_size + 2;
	
	data_size = read_slip_uart(data, data_size);
	
	if (data_size == sett_size + 2 && data[0] == KEY_1)//read settings
	{
		//crc chek
		crc_val = crc8(&data[1], sett_size);
		if (crc_val == data[sett_size + 1])
		{
			clear_flash(FLASH_ADRESS);
			write_data_to_flash(FLASH_ADRESS, &data[1], sett_size + 2);
			data[0] = KEY_OK;
		}
		else
			data[0] = KEY_ER;
		write_slip_uart(data, 1);
		return;
	}
	
	if (data_size == 1 && data[0] == KEY_2)//write settings
	{
		read_data_from_flash(FLASH_ADRESS, data, sett_size + 1);
		write_slip_uart(data, sett_size + 1); 
		return;
	}

	data[0] = KEY_ER;
	write_slip_uart(data, 1);
}
//------------------------------------------------------------------------------------------------------------------
uint32_t write_slip_uart(const uint8_t* data, uint32_t data_size)
{
	uint32_t i = 0;
	
	if (data == NULL)
		return 0;
	if (data_size > UART_MAX_DATA_SIZE)
		data_size = UART_MAX_DATA_SIZE;
	
	data_to_slip_buff(data, data_size);
	
	for (; i < uart_tx_counter; i++)
	{
		while (usart_flag_get(USARTx, USART_TDBE_FLAG) == RESET);
		usart_data_transmit(USARTx, uart_tx_buffer[i]);
	}
	uart_tx_counter = 0;
	
	return data_size;
}

uint32_t read_slip_uart(uint8_t* data, uint32_t data_size)
{
	if (data == NULL || RX_BUFFER_READY == FALSE)
		return 0;
	
	//RX_BUFFER_READY = FALSE;
	
	return slip_buff_to_data(data, data_size);
}
//------------------------------------------------------------------------------------------------------------------
void data_to_slip_buff(const uint8_t* data, uint32_t data_size)
{
	uint32_t i = 0;
	
	for(; i < data_size; i++)
	{
		switch(data[i])
		{
			case END_BYTE:
				uart_tx_buffer[uart_tx_counter++] = ESC_BYTE;
				uart_tx_buffer[uart_tx_counter++] = END_BYTE_STFF;
				break;
			case ESC_BYTE:
				uart_tx_buffer[uart_tx_counter++] = ESC_BYTE;
				uart_tx_buffer[uart_tx_counter++] = ESC_BYTE_STFF;
				break;
			default:
				uart_tx_buffer[uart_tx_counter++] = data[i];
		}
	}
	
	uart_tx_buffer[uart_tx_counter++] = END_BYTE;
}

uint32_t slip_buff_to_data(uint8_t* data, uint32_t data_size)
{
	uint32_t i = 0;
	uint32_t j =  0;
	
	while (uart_rx_buffer[j] != END_BYTE && i < data_size)
	{
		if (uart_rx_buffer[j] != ESC_BYTE)
			data[i++] = uart_rx_buffer[j++];
		else
		{
			j++;
			switch(uart_rx_buffer[j++])
			{
			case END_BYTE_STFF:
				data[i++] = END_BYTE;
				break;
			case ESC_BYTE_STFF:
				data[i++] = ESC_BYTE;
				break;
			default:
				data[i++] = uart_rx_buffer[j - 1];
			}
		}
	}
	
	return i;
}
//------------------------------------------------------------------------------------------------------------------
void USART1_IRQHandler(void)
{
	if (USARTx->ctrl1_bit.rdbfien != RESET)
	{
		if (usart_flag_get(USARTx, USART_RDBF_FLAG) != RESET)
		{
			RX_BUFFER_READY = FALSE;
			uart_rx_counter %= UART_BUFFER_SIZE;
			uart_rx_buffer[uart_rx_counter++] = usart_data_receive(USARTx);
			if (uart_rx_buffer[uart_rx_counter - 1] == END_BYTE)
			{
				uart_rx_counter = 0;
				RX_BUFFER_READY = TRUE;
				input_handler();
			}
		}
	}
}
//------------------------------------------------------------------------------------------------------------------
void slip_uart_configuration(void)
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
	nvic_irq_enable(USARTx_IRQn, 0, 0);
}

void debug_f(uint8_t kek)
{
	while (usart_flag_get(USART1, USART_TDBE_FLAG) == RESET);
	usart_data_transmit(USART1, kek);
}
