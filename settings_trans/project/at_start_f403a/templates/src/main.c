#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"

#include "settings_trans.h"

#define DELAY                            50 //100
#define FAST                             1
#define SLOW                             1 //4

uint8_t g_speed = FAST;

void button_exint_init(void);
void button_isr(void);
void plant_settings(uint32_t n_set, struct settings_str* sett);

//-----------------------------------------------------------------------
void EXINT0_IRQHandler(void)
{
	uint8_t data[sett_size];
	
	convert_sett_to_data(settings, data, sett_size);
	write_usart(data, sett_size);
	read_data_from_flash(FLASH_ADRESS, data, sett_size);
	write_usart(data, sett_size);
	
	button_isr();
}
//-----------------------------------------------------------------------
int main(void)
{
	system_clock_config();
	at32_board_init();
	button_exint_init();

	configurate_settings_trans();
	
	//plant_settings(settings);
	if (settings->rpm == 18000)//test
		debug_f(33);
		
	while(1)
	{
		at32_led_toggle(LED2);
		delay_ms(g_speed * DELAY);
		at32_led_toggle(LED3);
		delay_ms(g_speed * DELAY);
		at32_led_toggle(LED4);
		delay_ms(g_speed * DELAY);
	}
}
//-----------------------------------------------------------------------
/**
  * @brief  configure button exint
  * @param  none
  * @retval none
  */
void button_exint_init(void)
{
	exint_init_type exint_init_struct;

	crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);//?
	gpio_exint_line_config(GPIO_PORT_SOURCE_GPIOA, GPIO_PINS_SOURCE0);//?

	exint_default_para_init(&exint_init_struct);
	exint_init_struct.line_enable = TRUE;
	exint_init_struct.line_mode = EXINT_LINE_INTERRUPUT;
	exint_init_struct.line_select = EXINT_LINE_0;
	exint_init_struct.line_polarity = EXINT_TRIGGER_RISING_EDGE;
	exint_init(&exint_init_struct);

	nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
	nvic_irq_enable(EXINT0_IRQn, 0, 0);
}

/**
  * @brief  button handler function
  * @param  none
  * @retval none
  */
void button_isr(void)
{
	delay_ms(50);

	/* clear interrupt pending bit */
	exint_flag_clear(EXINT_LINE_0);

	/* check input pin state */
	if(SET == gpio_input_data_bit_read(USER_BUTTON_PORT, USER_BUTTON_PIN))
	{
		if(g_speed == SLOW)
			g_speed = FAST;
		else
			g_speed = SLOW;
	}
}

void plant_settings(uint32_t n_set, struct settings_str* sett)
{
	switch (n_set)
	{
		case 1:
			sett->rpm = 228;
			sett->direction = '0';
			sett->temperature = 0;
			sett->pressure = 3.14;
			break;
			//DATA
			//228 0 0 0 48 0 0 0 0 0 0 0 195 245 72 64
			//CRC
			//230
			//CRC of first 4 bytes
			//184
		case 2:
			sett->rpm = 10000;
			sett->direction = 'u';
			sett->temperature = 36;
			sett->pressure = 2.72;
			break;
			//DATA
			//16 39 0 0 117 0 0 0 36 0 0 0 123 20 46 64
			//CRC
			//148
			//CRC of first 4 bytes
			//117
		default:
			sett->rpm = 18000;
			sett->direction = 'd';
			sett->temperature = 67;
			sett->pressure = 288.666;
			//DATA
			//80 70 0 0 100 0 0 0 67 0 0 0 63 85 144 67
			//CRC
			//235
			//CRC of first 4 bytes
			//88
	}
}
