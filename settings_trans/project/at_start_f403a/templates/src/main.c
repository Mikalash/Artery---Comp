#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"

#include "SystemParams_tag.h"
#include "SystemParams_trans.h"

#define DELAY                            50 //100
#define FAST                             1
#define SLOW                             1  //4

uint8_t g_speed = FAST;

void gpio_configuration(void);
void button_exint_init(void);
void button_isr(void);

//-----------------------------------------------------------------------
void EXINT0_IRQHandler(void)
{
	uint8_t data[SP_shell_size];
	
	read_data_from_flash(FLASH_ADRESS, data, SP_shell_size);
	write_slip_uart(data, SP_shell_size);
	
	button_isr();
	
}
//-----------------------------------------------------------------------
int main(void)
{
	SystemParams_type SP;
	
	system_clock_config();
	at32_board_init();
	button_exint_init();

	gpio_configuration();
	
	init_SystemParams_type(&SP);
	configurate_SystemParams_trans();

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
	delay_ms(100);

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
