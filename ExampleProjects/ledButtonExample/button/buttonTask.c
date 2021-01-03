/***************************************************************************************************
 * filename     : micrOsTask_button.c
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

#include "buttonTask.h"
#include "micrOsTask_button.h"
#include "taskManagerFunctions.h"
#include "stm32l0xx_hal.h"
#include "ledTask.h"

#define PIN_BUTTON GPIO_PIN_2
#define PORT_BUTTON GPIOB

#define DEBOUNCE_TIME 50 //ms
#define LONG_PRESS_TIME 750 //ms

#define isButtonPressed !HAL_GPIO_ReadPin(PORT_BUTTON, PIN_BUTTON)

static void (*pfnCurrentButtonState)(void);
static void (*pfnPrevButtonState)(void);
static void (*pfnNextButtonState)(void);

static void buttonStateWaitPress(void);
static void buttonStateWaitUnpress(void);
static void buttonStateWaitLongPress(void);
static void buttonStateWaitDebounce(void);

static void buttonPressOp(void);
static void buttonShortPressOp(void);
static void buttonLongPressOp(void);
static void buttonUnpressOp(void);

static void getOnlyIdSignal(sSignalOnlyId *gettingSignal);
static void getSignalAndData(sSignalWithU8Data *gettingSignal);

static sSignalGeneral signalContainer;

static uint8_t *debounceTimerKey;
static uint8_t *longPressTimerKey;
static bool debounceTimeout = false;
static bool longPressTimeout = false;
static uint8_t selectedLed = UNSELECTED_LED;

void taskButtonMain(void)
{
	pfnCurrentButtonState();
}

void taskButtonInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitStruct.Pin = PIN_BUTTON;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(PORT_BUTTON, &GPIO_InitStruct);
	
	pfnCurrentButtonState = buttonStateWaitPress;
}

void taskButtonGetSignal(sSignalGeneral *signalVar)
{
	switch(signalVar->signalType)
    {
        case SIGNAL_TYPE__ONLY_ID:
        {
            sSignalOnlyId gettingSignal = *(sSignalOnlyId*)(signalVar->signalStruct);
						getOnlyIdSignal(&gettingSignal);
            break;
        }
        case SIGNAL_TYPE__WITH_U8_DATA:
        {
            sSignalWithU8Data gettingSignal = *(sSignalWithU8Data*)(signalVar->signalStruct);
            getSignalAndData(&gettingSignal);
            break;
        }
    }
}

static void getOnlyIdSignal(sSignalOnlyId *gettingSignal)
{
	switch(gettingSignal->signalId)
	{
		case SIGNAL_ID__DEBOUNCE_TIMEOUT:
			debounceTimeout = true;
		break;
		case SIGNAL_ID__LONG_PRESS_TIMEOUT:
			longPressTimeout = true;
		break;
		default:
			break;
	}
}

static void getSignalAndData(sSignalWithU8Data *gettingSignal)
{

}

static void buttonPressOp(void)
{
 
}

static void buttonShortPressOp(void)
{
	if(selectedLed != UNSELECTED_LED)
	{
		signalContainer.signalType = SIGNAL_TYPE__WITH_U8_DATA;
		sSignalWithU8Data signalAndLedNo = {SIGNAL_ID__LED_MODE_CHANGED,selectedLed};
		signalContainer.signalStruct = &signalAndLedNo;
		micrOs_publishEventToSubscribers(EVENT_TYPE__LED_MODE,&signalContainer);
	}
}
	
static void buttonLongPressOp(void)
{
	if(selectedLed == LED3_SELECTED)
		selectedLed = UNSELECTED_LED;
	else
		selectedLed++;
	signalContainer.signalType = SIGNAL_TYPE__WITH_U8_DATA;
	sSignalWithU8Data signalAndLedNo = {SIGNAL_ID__SELECTED_LED_CHANGED,selectedLed};
	signalContainer.signalStruct = &signalAndLedNo;
	micrOs_publishEventToSubscribers(EVENT_TYPE__LED_MODE,&signalContainer);
}

static void buttonUnpressOp(void)
{

}

void buttonStateWaitPress(void)
{
	if(isButtonPressed)
	{
		signalContainer.signalType = SIGNAL_TYPE__ONLY_ID;
		sSignalOnlyId OnlySignal = {SIGNAL_ID__DEBOUNCE_TIMEOUT};
		signalContainer.signalStruct = &OnlySignal;
		debounceTimeout = false;
		debounceTimerKey = micrOs_startEventDispachTimer(MICROS_TIMER_TYPE_ONE_SHOT,DEBOUNCE_TIME,TASK_ID__BUTTON,&signalContainer);
		pfnCurrentButtonState = buttonStateWaitDebounce;
		pfnPrevButtonState = buttonStateWaitPress;
		pfnNextButtonState = buttonStateWaitLongPress;
	}
}

void buttonStateWaitUnpress(void)
{
	if(!isButtonPressed)
	{
		signalContainer.signalType = SIGNAL_TYPE__ONLY_ID;
		sSignalOnlyId OnlySignal = {SIGNAL_ID__DEBOUNCE_TIMEOUT};
		signalContainer.signalStruct = &OnlySignal;
		debounceTimeout = false;
		debounceTimerKey = micrOs_startEventDispachTimer(MICROS_TIMER_TYPE_ONE_SHOT,DEBOUNCE_TIME,TASK_ID__BUTTON,&signalContainer);
		pfnCurrentButtonState = buttonStateWaitDebounce;
		pfnPrevButtonState = buttonStateWaitUnpress;
		pfnNextButtonState = buttonStateWaitPress;
	}
}

void buttonStateWaitLongPress(void)
{
	if(longPressTimeout)
	{
		pfnCurrentButtonState = buttonStateWaitUnpress;
		// button long pressed
		buttonLongPressOp();
		return;
	}
	buttonStateWaitUnpress();
	pfnPrevButtonState = buttonStateWaitLongPress;
}

void buttonStateWaitDebounce(void)
{
	if(debounceTimeout)
	{
		pfnCurrentButtonState = pfnNextButtonState;
		if(pfnPrevButtonState == buttonStateWaitLongPress)
		{
			if(!longPressTimeout)
				micrOs_cancelTimer(longPressTimerKey);
			// button short press
			buttonShortPressOp();
		}
		else if (pfnNextButtonState == buttonStateWaitLongPress)
		{
			signalContainer.signalType = SIGNAL_TYPE__ONLY_ID;
			sSignalOnlyId OnlySignal = {SIGNAL_ID__LONG_PRESS_TIMEOUT};
			signalContainer.signalStruct = &OnlySignal;
			longPressTimeout = false;
			longPressTimerKey = micrOs_startEventDispachTimer(MICROS_TIMER_TYPE_ONE_SHOT,LONG_PRESS_TIME-DEBOUNCE_TIME,TASK_ID__BUTTON,&signalContainer);
			// button pressed
			buttonPressOp();
		}
		if(pfnNextButtonState == buttonStateWaitPress)
		{
			// button unpressed
			buttonUnpressOp();
		}
	}
	// button state changed in debounce time
	if((pfnPrevButtonState == buttonStateWaitPress && !isButtonPressed) ||
		((pfnPrevButtonState == buttonStateWaitLongPress || pfnPrevButtonState == buttonStateWaitLongPress) && isButtonPressed))
	{
		micrOs_cancelTimer(debounceTimerKey);
		pfnCurrentButtonState = pfnPrevButtonState;
	}
}
