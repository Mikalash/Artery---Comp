#pragma once

#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"

//uart configuration
#define USARTx USART1
#define USARTx_IRQn USART1_IRQn //if change this - change name of function USART1_IRQHandler
#define CRM_USARTx_PERIPH_CLOCK CRM_USART1_PERIPH_CLOCK
#define USART_TX_PIN GPIO_PINS_9
#define USART_RX_PIN GPIO_PINS_10
#define USART_BOUD 9600

//uart max data len
//you can define yout own size before include slip_uart
#ifndef UART_MAX_DATA_SIZE
	#define UART_MAX_DATA_SIZE 256
#endif
//uart buffers len
#define UART_BUFFER_SIZE (UART_MAX_DATA_SIZE * 2 + 1)

//byte stuffing
#define END_BYTE 192
#define ESC_BYTE 219
#define END_BYTE_STFF 220
#define ESC_BYTE_STFF 221

//------------------------------------------------------------------------------------------------------------------
uint32_t write_slip_uart(const uint8_t* data, uint32_t data_size);

uint32_t read_slip_uart(uint8_t* data, uint32_t data_size);

//------------------------------------------------------------------------------------------------------------------
void USART1_IRQHandler(void);

//------------------------------------------------------------------------------------------------------------------
void slip_uart_configuration(void (*user_rx_handler)(void));

//------------------------------------------------------------------------------------------------------------------
void debug_f(uint8_t kek);
