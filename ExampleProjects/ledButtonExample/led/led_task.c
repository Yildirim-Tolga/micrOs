/***************************************************************************************************
 * filename     : micrOsTask_led.c
 * description  :
 * author       : Tolga Yildirim
 * copyright    :
 * datetime     :
 *
 * Version History:  +,-,/,*: add, remove, change, fix
 *  v1.0:   First release.
 *
 *
 **************************************************************************************************/

#include "led_task.h"
#include "micros_task_led.h"
#include "micros.h"
#include "stm32l0xx_hal.h"

#define PIN_LED_1 GPIO_PIN_5
#define PORT_LED_1 GPIOB
#define PIN_LED_2 GPIO_PIN_6
#define PORT_LED_2 GPIOB
#define PIN_LED_3 GPIO_PIN_7
#define PORT_LED_3 GPIOB
#define PIN_LED_SEL GPIO_PIN_5
#define PORT_LED_SEL GPIOA

#define PIN_LED(x) (x == 1 ? PIN_LED_1 : (x == 2 ? PIN_LED_2 : (x == 3 ? PIN_LED_3 : PIN_LED_SEL)))
#define PORT_LED(x) (x == 1 ? PORT_LED_1 : (x == 2 ? PORT_LED_2 : (x == 3 ? PORT_LED_3 : PORT_LED_SEL)))

enum
{
	LED_OFF,
	LED_BLINK_100MS,
	LED_BLINK_1S,
	LED_HEART_BEAT,
	LED_ON,
	LED_MODE_COUNT
};

static void led_get_signal_id(sSig_id *getting_signal);
static void led_get_signal_id_u8(sSig_id_u8 *getting_signal);

static sSig_gen signalContainer;

uint8_t tm_key_led[3], tm_key_led_select;
uint8_t selected_led = UNSELECTED_LED;
uint8_t led_mode[3] = {LED_OFF, LED_OFF, LED_OFF};
uint8_t heart_beat_cnt[3] = {0, 0, 0};

void led_main(void)
{
}

void led_init(void)
{
	// GPIO Initialize
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitStruct.Pin = PIN_LED_SEL;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(PORT_LED_SEL, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = PIN_LED_1;
	HAL_GPIO_Init(PORT_LED_1, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = PIN_LED_2;
	HAL_GPIO_Init(PORT_LED_2, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = PIN_LED_3;
	HAL_GPIO_Init(PORT_LED_3, &GPIO_InitStruct);

	HAL_GPIO_WritePin(PORT_LED_SEL, PIN_LED_SEL, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PORT_LED_1, PIN_LED_1, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PORT_LED_2, PIN_LED_2, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PORT_LED_3, PIN_LED_3, GPIO_PIN_RESET);
	// Set subscription state
	micros_event_substate_set(EVENT_TYPE__LED_MODE, TASK_ID__LED, 1);
}

void led_get_signal(sSig_gen *signal)
{
	switch (signal->sig_type)
	{
	case SIGNAL_TYPE__ID:
	{
		sSig_id getting_signal = *(sSig_id *)(signal->ptr_sig);
		led_get_signal_id(&getting_signal);
		break;
	}
	case SIGNAL_TYPE__ID_U8:
	{
		sSig_id_u8 getting_signal = *(sSig_id_u8 *)(signal->ptr_sig);
		led_get_signal_id_u8(&getting_signal);
		break;
	}
	default:
		break;
	}
}

static void led_get_signal_id(sSig_id *getting_signal)
{
	switch (getting_signal->sig_id)
	{
	case SIGNAL_ID__LED_SEL_TIMEOUT:
		HAL_GPIO_TogglePin(PORT_LED_SEL, PIN_LED_SEL);
		break;
	case SIGNAL_ID__LED1_TIMEOUT:
		if (led_mode[0] == LED_HEART_BEAT)
		{
			heart_beat_cnt[0]++;
			if (heart_beat_cnt[0] <= 4)
				HAL_GPIO_TogglePin(PORT_LED_1, PIN_LED_1);
			if (heart_beat_cnt[0] >= 20)
				heart_beat_cnt[0] = 0;
		}
		else
			HAL_GPIO_TogglePin(PORT_LED_1, PIN_LED_1);
		break;

	case SIGNAL_ID__LED2_TIMEOUT:
		if (led_mode[1] == LED_HEART_BEAT)
		{
			heart_beat_cnt[1]++;
			if (heart_beat_cnt[1] <= 4)
				HAL_GPIO_TogglePin(PORT_LED_2, PIN_LED_2);
			if (heart_beat_cnt[1] >= 20)
				heart_beat_cnt[1] = 0;
		}
		else
			HAL_GPIO_TogglePin(PORT_LED_2, PIN_LED_2);
		break;

	case SIGNAL_ID__LED3_TIMEOUT:
		if (led_mode[2] == LED_HEART_BEAT)
		{
			heart_beat_cnt[2]++;
			if (heart_beat_cnt[2] <= 4)
				HAL_GPIO_TogglePin(PORT_LED_3, PIN_LED_3);
			if (heart_beat_cnt[2] >= 20)
				heart_beat_cnt[2] = 0;
		}
		else
			HAL_GPIO_TogglePin(PORT_LED_3, PIN_LED_3);
		break;
	default:
		break;
	}
}

static void led_get_signal_id_u8(sSig_id_u8 *getting_signal)
{
	switch (getting_signal->sig_id)
	{
	case SIGNAL_ID__SELECTED_LED_CHANGED:
		if (selected_led == LED2_SELECTED || selected_led == LED3_SELECTED)
			micros_timer_cancel(tm_key_led_select);
		selected_led = getting_signal->data;
		if (selected_led == UNSELECTED_LED)
		{
			HAL_GPIO_WritePin(PORT_LED_SEL, PIN_LED_SEL, GPIO_PIN_RESET);
		}
		else if (selected_led == LED1_SELECTED)
		{
			HAL_GPIO_WritePin(PORT_LED_SEL, PIN_LED_SEL, GPIO_PIN_SET);
		}
		else
		{
			HAL_GPIO_TogglePin(PORT_LED_SEL, PIN_LED_SEL);

			uint16_t blinkTime = 100;
			if (selected_led == LED3_SELECTED)
				blinkTime = 1000;

			signalContainer.sig_type = SIGNAL_TYPE__ID;
			sSig_id signal_only = {SIGNAL_ID__LED_SEL_TIMEOUT};
			signalContainer.ptr_sig = &signal_only;
			tm_key_led_select = micros_timer_event_dispatch(MICROS_TM_TYPE_PERIODIC, 1, blinkTime, TASK_ID__LED, &signalContainer);
		}
		break;
	case SIGNAL_ID__LED_MODE_CHANGED:
	{
		uint8_t ledNo = getting_signal->data;
		uint8_t *currentMode = &led_mode[getting_signal->data - 1];
		if (*currentMode == LED_BLINK_100MS || *currentMode == LED_BLINK_1S || *currentMode == LED_HEART_BEAT)
		{
			micros_timer_cancel(tm_key_led[getting_signal->data - 1]);
		}
		(*currentMode)++;
		if (*currentMode == LED_MODE_COUNT)
		{
			*currentMode = LED_OFF;
			HAL_GPIO_WritePin(PORT_LED(ledNo), PIN_LED(ledNo), GPIO_PIN_RESET);
		}
		else if (*currentMode == LED_ON)
		{
			HAL_GPIO_WritePin(PORT_LED(ledNo), PIN_LED(ledNo), GPIO_PIN_SET);
		}
		else
		{
			if (*currentMode == LED_HEART_BEAT)
				HAL_GPIO_WritePin(PORT_LED(ledNo), PIN_LED(ledNo), GPIO_PIN_RESET);
			else
				HAL_GPIO_TogglePin(PORT_LED(ledNo), PIN_LED(ledNo));
			uint16_t blinkTime = 100;
			if (*currentMode == LED_BLINK_1S)
				blinkTime = 1000;
			else if (*currentMode == LED_HEART_BEAT)
				blinkTime = 200;
			heart_beat_cnt[ledNo - 1] = 0;
			signalContainer.sig_type = SIGNAL_TYPE__ID;
			sSig_id signal_only = {(eSignalId)(SIGNAL_ID__LED_SEL_TIMEOUT + ledNo)};
			signalContainer.ptr_sig = &signal_only;
			tm_key_led[ledNo - 1] = micros_timer_event_dispatch(MICROS_TM_TYPE_PERIODIC, 1, blinkTime, TASK_ID__LED, &signalContainer);
		}
	}
	break;
	default:
		break;
	}
}
