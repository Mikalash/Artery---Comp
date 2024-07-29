#include "slip_uart.h"

//------------------------------------------------------------------------------------------------------------------
//false when slip_uart was initialized
static _Bool no_slip_uart_init = TRUE;

//buffer with stuffed data bytes and END_BYTE wich go to UART
volatile static uint8_t uart_tx_buffer[UART_BUFFER_SIZE];
volatile static uint32_t uart_tx_counter = 0;

//buffer with stuffed data bytes and END_BYTE wich get from UART
volatile static uint8_t uart_rx_buffer[UART_BUFFER_SIZE];
volatile static uint32_t uart_rx_counter = 0;

//true when get END_BYTE from UART
volatile static _Bool RX_BUFFER_READY = FALSE;
//END_BYTE read handler function
static void (*rx_handler)(void) = NULL;

//------------------------------------------------------------------------------------------------------------------
//function wich make byte stuffing inside write_slip_uart
static void data_to_slip_buff(const uint8_t* data, uint32_t data_size);
//function wich make reverce byte stuffing inside read_slip_uart
static uint32_t slip_buff_to_data(uint8_t* data, uint32_t data_size);

//------------------------------------------------------------------------------------------------------------------
void slip_uart_configuration(void (*user_rx_handler)(void))
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
	
	rx_handler = user_rx_handler;
	
	no_slip_uart_init = FALSE;
}

//------------------------------------------------------------------------------------------------------------------
uint32_t write_slip_uart(const uint8_t* data, uint32_t data_size)
{
	uint32_t i = 0;
	
	if (data == NULL  || no_slip_uart_init)
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
	if (data == NULL || RX_BUFFER_READY == FALSE || no_slip_uart_init)
		return 0;
	
	return slip_buff_to_data(data, data_size);
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
				if (rx_handler != NULL)
					rx_handler();
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------
static void data_to_slip_buff(const uint8_t* data, uint32_t data_size)
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

static uint32_t slip_buff_to_data(uint8_t* data, uint32_t data_size)
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

void debug_f(uint8_t kek)
{
	if (no_slip_uart_init)
		return;
	
	while (usart_flag_get(USART1, USART_TDBE_FLAG) == RESET);
	usart_data_transmit(USART1, kek);
}
