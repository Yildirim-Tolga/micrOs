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

#include "ledTask.h"
#include "micrOsTask_led.h"
#include "taskManagerFunctions.h"
#include "stm32l0xx_hal.h"

#define PIN_LED_1 GPIO_PIN_5
#define PORT_LED_1 GPIOB
#define PIN_LED_2 GPIO_PIN_6
#define PORT_LED_2 GPIOB
#define PIN_LED_3 GPIO_PIN_7
#define PORT_LED_3 GPIOB
#define PIN_LED_SEL GPIO_PIN_5
#define PORT_LED_SEL GPIOA

#define PIN_LED(x) (x == 1 ? PIN_LED_1 : (x == 2 ? PIN_LED_2 : (x == 3 ? PIN_LED_3: PIN_LED_SEL)))
#define PORT_LED(x) (x == 1 ? PORT_LED_1 : (x == 2 ? PORT_LED_2 : (x == 3 ? PORT_LED_3: PORT_LED_SEL)))

enum
{
	LED_OFF,
	LED_BLINK_100MS,
	LED_BLINK_1S,
	LED_HEART_BEAT,
	LED_ON,
	LED_MODE_COUNT
};

static void getOnlyIdSignal(sSignalOnlyId *gettingSignal);
static void getSignalAndData(sSignalWithU8Data *gettingSignal);

static sSignalGeneral signalContainer;

uint8_t *ledTimerKey[3],*ledSelTimerKey;
uint8_t selectedLed = UNSELECTED_LED;
uint8_t ledMode[3] = {LED_OFF,LED_OFF,LED_OFF};
uint8_t heartBeatCnt[3] = {0,0,0};

void taskLedMain(void)
{

}

void taskLedInit(void)
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
	micrOs_setEventSubscriptionState(EVENT_TYPE__LED_MODE,TASK_ID__LED,1);
}

void taskLedGetSignal(sSignalGeneral *signalVar)
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
		case SIGNAL_ID__LED_SEL_TIMEOUT:
			HAL_GPIO_TogglePin(PORT_LED_SEL, PIN_LED_SEL);
		break;
		case SIGNAL_ID__LED1_TIMEOUT:
			if(ledMode[0] == LED_HEART_BEAT)
			{
				heartBeatCnt[0]++;
				if(heartBeatCnt[0] <= 4)
					HAL_GPIO_TogglePin(PORT_LED_1, PIN_LED_1);
				if(heartBeatCnt[0] >=20)
					heartBeatCnt[0] = 0;
			}
			else
				HAL_GPIO_TogglePin(PORT_LED_1, PIN_LED_1);
		break;
			
		case SIGNAL_ID__LED2_TIMEOUT:
			if(ledMode[1] == LED_HEART_BEAT)
			{
				heartBeatCnt[1]++;
				if(heartBeatCnt[1] <= 4)
					HAL_GPIO_TogglePin(PORT_LED_2, PIN_LED_2);
				if(heartBeatCnt[1] >=20)
					heartBeatCnt[1] = 0;
			}
			else
				HAL_GPIO_TogglePin(PORT_LED_2, PIN_LED_2);
		break;
			
		case SIGNAL_ID__LED3_TIMEOUT:
			if(ledMode[2] == LED_HEART_BEAT)
			{
				heartBeatCnt[2]++;
				if(heartBeatCnt[2] <= 4)
					HAL_GPIO_TogglePin(PORT_LED_3, PIN_LED_3);
				if(heartBeatCnt[2] >=20)
					heartBeatCnt[2] = 0;
			}
			else
				HAL_GPIO_TogglePin(PORT_LED_3, PIN_LED_3);
		break;
		default:
			break;
	}
}

static void getSignalAndData(sSignalWithU8Data *gettingSignal)
{
	switch(gettingSignal->signalId)
	{
		case SIGNAL_ID__SELECTED_LED_CHANGED:
			if(selectedLed == LED2_SELECTED || selectedLed == LED3_SELECTED)
				micrOs_cancelTimer(ledSelTimerKey);
			selectedLed = gettingSignal->data;
			if(selectedLed == UNSELECTED_LED)
			{
				HAL_GPIO_WritePin(PORT_LED_SEL, PIN_LED_SEL, GPIO_PIN_RESET);
			}
			else if(selectedLed == LED1_SELECTED)
			{
				HAL_GPIO_WritePin(PORT_LED_SEL, PIN_LED_SEL, GPIO_PIN_SET);
			}
			else
			{
				HAL_GPIO_TogglePin(PORT_LED_SEL, PIN_LED_SEL);
				
				uint16_t blinkTime = 100;
				if(selectedLed == LED3_SELECTED)
					blinkTime = 1000;
				
				signalContainer.signalType = SIGNAL_TYPE__ONLY_ID;
				sSignalOnlyId OnlySignal = {SIGNAL_ID__LED_SEL_TIMEOUT};
				signalContainer.signalStruct = &OnlySignal;
				ledSelTimerKey = micrOs_startEventDispachTimer(MICROS_TIMER_TYPE_PERIODIC,blinkTime,TASK_ID__LED,&signalContainer);
			}
		break;
		case SIGNAL_ID__LED_MODE_CHANGED:
		{
			uint8_t ledNo = gettingSignal->data;
			uint8_t *currentMode = &ledMode[gettingSignal->data - 1];
			if(*currentMode == LED_BLINK_100MS || *currentMode == LED_BLINK_1S || *currentMode == LED_HEART_BEAT)
			{
				micrOs_cancelTimer(ledTimerKey[gettingSignal->data - 1]);
			}
			(*currentMode)++;
			if(*currentMode == LED_MODE_COUNT)
			{
				*currentMode = LED_OFF;
				HAL_GPIO_WritePin(PORT_LED(ledNo), PIN_LED(ledNo), GPIO_PIN_RESET);
			}
			else if(*currentMode == LED_ON)
			{
				HAL_GPIO_WritePin(PORT_LED(ledNo), PIN_LED(ledNo), GPIO_PIN_SET);
			}
			else
			{
				if(*currentMode == LED_HEART_BEAT)
					HAL_GPIO_WritePin(PORT_LED(ledNo), PIN_LED(ledNo), GPIO_PIN_RESET);
				else
					HAL_GPIO_TogglePin(PORT_LED(ledNo), PIN_LED(ledNo));
				uint16_t blinkTime = 100;
				if(*currentMode == LED_BLINK_1S)
					blinkTime = 1000;
				else if (*currentMode == LED_HEART_BEAT)
					blinkTime = 200;
				heartBeatCnt[ledNo-1] = 0;
				signalContainer.signalType = SIGNAL_TYPE__ONLY_ID;
				sSignalOnlyId OnlySignal = {(eSignalId)(SIGNAL_ID__LED_SEL_TIMEOUT+ledNo)};
				signalContainer.signalStruct = &OnlySignal;
				ledTimerKey[ledNo-1] = micrOs_startEventDispachTimer(MICROS_TIMER_TYPE_PERIODIC,blinkTime,TASK_ID__LED,&signalContainer);
			}
		}
		break;
		default:
			break;
	}
}
