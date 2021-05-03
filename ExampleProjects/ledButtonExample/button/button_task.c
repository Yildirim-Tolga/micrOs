/**
 * @file button_task.c
 * @author Tolga Yildirim (tolga.yildirimmm@gmail.com)
 * @brief 
 * @version 1.0
 * @date 2021-05-03
 * 
 * @copyright
 * 
 * Version History:  +,-,/,*: add, remove, change, fix
 *  v1.0:   First release.
 */

#include "button_task.h"
#include "micros_task_button.h"
#include "micros.h"
#include "stm32l0xx_hal.h"
#include "led_task.h"

#define PIN_BUTTON GPIO_PIN_2
#define PORT_BUTTON GPIOB

#define DEBOUNCE_TIME 50	//ms
#define LONG_PRESS_TIME 750 //ms

#define isButtonPressed !HAL_GPIO_ReadPin(PORT_BUTTON, PIN_BUTTON)

static void (*pfn_current_button_state)(void);
static void (*pfn_prev_button_state)(void);
static void (*pfn_next_button_state)(void);

static void button_state_wait_press(void);
static void button_state_wait_release(void);
static void button_state_wait_long_press(void);
static void button_state_wait_debounce(void);

static void button_press_operation(void);
static void button_short_press_operation(void);
static void button_long_press_operation(void);
static void button_release_operation(void);

static void button_get_signal_id(sSig_id *getting_signal);
static void button_get_signal_id_u8(sSig_id_u8 *getting_signal);

static sSig_gen signal_container;

static uint8_t tm_key_debounce;
static uint8_t tm_key_long_press;
static bool timeout_debounce = false;
static bool timeout_long_press = false;
static uint8_t selected_led = UNSELECTED_LED;

void button_main(void)
{
	pfn_current_button_state();
}

void button_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitStruct.Pin = PIN_BUTTON;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(PORT_BUTTON, &GPIO_InitStruct);

	pfn_current_button_state = button_state_wait_press;
}

void button_get_signal(sSig_gen *signal)
{
	switch (signal->sig_type)
	{
	case SIGNAL_TYPE__ID:
	{
		sSig_id getting_signal = *(sSig_id *)(signal->ptr_sig);
		button_get_signal_id(&getting_signal);
		break;
	}
	case SIGNAL_TYPE__ID_U8:
	{
		sSig_id_u8 getting_signal = *(sSig_id_u8 *)(signal->ptr_sig);
		button_get_signal_id_u8(&getting_signal);
		break;
	}
	default:
		break;
	}
}

static void button_get_signal_id(sSig_id *getting_signal)
{
	switch (getting_signal->sig_id)
	{
	case SIGNAL_ID__DEBOUNCE_TIMEOUT:
		timeout_debounce = true;
		break;
	case SIGNAL_ID__LONG_PRESS_TIMEOUT:
		timeout_long_press = true;
		break;
	default:
		break;
	}
}

static void button_get_signal_id_u8(sSig_id_u8 *getting_signal)
{
}

static void button_press_operation(void)
{
}

static void button_short_press_operation(void)
{
	if (selected_led != UNSELECTED_LED)
	{
		signal_container.sig_type = SIGNAL_TYPE__ID_U8;
		sSig_id_u8 signal_and_led_no = {SIGNAL_ID__LED_MODE_CHANGED, selected_led};
		signal_container.ptr_sig = &signal_and_led_no;
		micros_event_publish(EVENT_TYPE__LED_MODE, &signal_container);
	}
}

static void button_long_press_operation(void)
{
	if (selected_led == LED3_SELECTED)
		selected_led = UNSELECTED_LED;
	else
		selected_led++;
	signal_container.sig_type = SIGNAL_TYPE__ID_U8;
	sSig_id_u8 signal_and_led_no = {SIGNAL_ID__SELECTED_LED_CHANGED, selected_led};
	signal_container.ptr_sig = &signal_and_led_no;
	micros_event_publish(EVENT_TYPE__LED_MODE, &signal_container);
}

static void button_release_operation(void)
{
}

void button_state_wait_press(void)
{
	if (isButtonPressed)
	{
		signal_container.sig_type = SIGNAL_TYPE__ID;
		sSig_id signal_only = {SIGNAL_ID__DEBOUNCE_TIMEOUT};
		signal_container.ptr_sig = &signal_only;
		timeout_debounce = false;
		tm_key_debounce = micros_timer_event_dispatch(MICROS_TM_TYPE_ONE_SHOT, 1, DEBOUNCE_TIME, TASK_ID__BUTTON, &signal_container);
		pfn_current_button_state = button_state_wait_debounce;
		pfn_prev_button_state = button_state_wait_press;
		pfn_next_button_state = button_state_wait_long_press;
	}
}

void button_state_wait_release(void)
{
	if (!isButtonPressed)
	{
		signal_container.sig_type = SIGNAL_TYPE__ID;
		sSig_id signal_only = {SIGNAL_ID__DEBOUNCE_TIMEOUT};
		signal_container.ptr_sig = &signal_only;
		timeout_debounce = false;
		tm_key_debounce = micros_timer_event_dispatch(MICROS_TM_TYPE_ONE_SHOT, 1, DEBOUNCE_TIME, TASK_ID__BUTTON, &signal_container);
		pfn_current_button_state = button_state_wait_debounce;
		pfn_prev_button_state = button_state_wait_release;
		pfn_next_button_state = button_state_wait_press;
	}
}

void button_state_wait_long_press(void)
{
	if (timeout_long_press)
	{
		pfn_current_button_state = button_state_wait_release;
		// button long pressed
		button_long_press_operation();
		return;
	}
	button_state_wait_release();
	pfn_prev_button_state = button_state_wait_long_press;
}

void button_state_wait_debounce(void)
{
	if (timeout_debounce)
	{
		pfn_current_button_state = pfn_next_button_state;
		if (pfn_prev_button_state == button_state_wait_long_press)
		{
			if (!timeout_long_press)
				micros_timer_cancel(tm_key_long_press);
			// button short press
			button_short_press_operation();
		}
		else if (pfn_next_button_state == button_state_wait_long_press)
		{
			signal_container.sig_type = SIGNAL_TYPE__ID;
			sSig_id signal_only = {SIGNAL_ID__LONG_PRESS_TIMEOUT};
			signal_container.ptr_sig = &signal_only;
			timeout_long_press = false;
			tm_key_long_press = micros_timer_event_dispatch(MICROS_TM_TYPE_ONE_SHOT, 1, LONG_PRESS_TIME - DEBOUNCE_TIME, TASK_ID__BUTTON, &signal_container);
			// button pressed
			button_press_operation();
		}
		if (pfn_next_button_state == button_state_wait_press)
		{
			// button unpressed
			button_release_operation();
		}
	}
	// button state changed in debounce time
	if ((pfn_prev_button_state == button_state_wait_press && !isButtonPressed) ||
		((pfn_prev_button_state == button_state_wait_long_press || pfn_prev_button_state == button_state_wait_long_press) && isButtonPressed))
	{
		micros_timer_cancel(tm_key_debounce);
		pfn_current_button_state = pfn_prev_button_state;
	}
}
