#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"

#include "settings_trans.h"

#define DELAY                            50 //100
#define FAST                             1
#define SLOW                             1 //4

uint8_t g_speed = FAST;

void button_exint_init(void);
void button_isr(void);
void plant_test_data(void);
void plant_settings(void);
void plant_settings_anth(void);

//-----------------------------------------------------------------------
void EXINT0_IRQHandler(void)
{
	uint8_t data[sett_size];
	
  button_isr();
	
	read_data_from_flash(FLASH_ADRESS, data, sett_size);
	write_usart(data, sett_size);
	
	//test for Evgeniy
	//write_usart(test_data, test_data_size);
	
	//delay_ms(g_speed * DELAY * 10);
}
//-----------------------------------------------------------------------
int main(void)
{
  system_clock_config();
  at32_board_init();
  button_exint_init();

	configurate_settings_trans();
	
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

void plant_test_data(void)
{
	if (test_data_size > 0)
		test_data[0] = 228;
	if (test_data_size > 1)
		test_data[1] = 111;
	if (test_data_size > 2)
		test_data[2] = 118;
	if (test_data_size > 3)
		test_data[3] = 16;
	if (test_data_size > 4)
		test_data[4] = 204;
}

void plant_settings(void)
{
	settings.rpm = 18000;
	settings.direction = 'd';
	settings.temperature = 67;
	settings.pressure = 288.666;
}

void plant_settings_anth(void)
{
	settings.rpm = 228;
	settings.direction = '0';
	settings.temperature = 0;
	settings.pressure = 3.14;
}
