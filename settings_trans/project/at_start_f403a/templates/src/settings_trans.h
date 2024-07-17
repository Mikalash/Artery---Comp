#pragma once

#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"

//configuration
#define FLASH_ADRESS 0x080FF800

#define USARTx USART1
#define CRM_USARTx_PERIPH_CLOCK CRM_USART1_PERIPH_CLOCK
#define USART_TX_PIN GPIO_PINS_9
#define USART_RX_PIN GPIO_PINS_10
#define USARTx_IRQn USART1_IRQn
#define USART_BOUD 9600
//consts
#define LEAD_BYTE_DF 127
#define LEAD_BYTE_RD 255
#define LEAD_BYTE_WR 0

#define END_BYTE_DF 127
#define END_BYTE_CRC_OK 255
#define END_BYTE_CRC_ER 0

//structure with settings and its size
struct settings_str
{
	int rpm;
	char direction;
	int temperature;
	float pressure;
};

#define sett_size sizeof(struct settings_str)
//#define sett_size 5 //for debug

static struct settings_str settings;

//for tests
static uint32_t test_data_size = 5;
static uint8_t test_data[5];
//for debug
void debug_f(uint8_t kek);
static uint8_t debug_data[63];
void debug_buf(const uint8_t* data, const uint8_t data_size);
//------------------------------------------------------------------------------------------------------------------
error_status convert_sett_to_data(const struct settings_str* sett, uint8_t* data, const uint32_t data_size);

error_status convert_data_to_sett(struct settings_str* sett, const uint8_t* data, const uint32_t data_size);
//------------------------------------------------------------------------------------------------------------------
error_status write_sett_to_flash(const struct settings_str* sett);

void read_sett_from_flash(struct settings_str* sett);
//------------------------------------------------------------------------------------------------------------------
error_status write_data_to_flash(uint32_t write_addr, const uint8_t* data, uint32_t data_size);

void read_data_from_flash(uint32_t read_addr, uint8_t* data, const uint32_t data_size);
//------------------------------------------------------------------------------------------------------------------
void write_usart(const uint8_t* data, const uint32_t data_size);

void read_usart(uint8_t* data, const uint32_t data_size);
//------------------------------------------------------------------------------------------------------------------
void USART1_IRQHandler(void);
//------------------------------------------------------------------------------------------------------------------
uint8_t crc8(const uint8_t* data, uint32_t data_size);
//------------------------------------------------------------------------------------------------------------------
void configurate_settings_trans(void);
void gpio_configuration(void);
void uart_configuration(void);
void crc_configuration(void);
