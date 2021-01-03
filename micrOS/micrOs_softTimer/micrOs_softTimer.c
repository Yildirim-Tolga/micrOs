/***************************************************************************************************
 * filename     : micrOs_softTimer.c
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
#include "micrOs_softTimer.h"
#include "../../errorHandler/errorHandler.h"
#include <stddef.h>
#include <stdlib.h>

volatile uint32_t timeCounter;
static sTimerList *timers;
static sTimerList *timersUseWithCallbackFunc;

static bool callStackInDeleteTimer = false;
static bool reCallCallbackTimer = false;
static bool callStackInCreateTimer = false;

void controlTimeout(sTimerList *timerList);

bool microsSofttimer_createTimer(sMicrOs_Timer **timer,uint8_t callbackType)
{
    callStackInCreateTimer = true;
    sTimerList **ppListOfTimer = &timers;
    if(callbackType == TIMER_CALLBACK_TYPE_CALLBACK_FUNC)
        ppListOfTimer = &timersUseWithCallbackFunc;
    while((*ppListOfTimer) != NULL)
    {
        ppListOfTimer = (sTimerList**)&(*ppListOfTimer)->next;
    }
    // allocate memory for new (next) timer list
    *ppListOfTimer = malloc(sizeof(sTimerList));
	(*ppListOfTimer)->next = NULL;
    (*ppListOfTimer)->timer.runState = false;
    if(*ppListOfTimer == NULL)
    {
        callStackInCreateTimer = false;
        if(reCallCallbackTimer)
            microsSofttimer_mcuTimerCallback(0);
        errorHandler(ERR_CODE_MALLOC_TIMER_LIST);
        return false;
    }
    (*ppListOfTimer)->timer.pTimerKey = malloc(sizeof(uint8_t));
    if((*ppListOfTimer)->timer.pTimerKey == NULL)
    {
        callStackInCreateTimer = false;
        if(reCallCallbackTimer)
            microsSofttimer_mcuTimerCallback(0);
        errorHandler(ERR_CODE_MALLOC_TIMER_KEY);
        return false;
    }
    (*ppListOfTimer)->timer.startingTime = timeCounter;
    *timer = &((*ppListOfTimer)->timer);
    callStackInCreateTimer = false;
    if(reCallCallbackTimer)
        microsSofttimer_mcuTimerCallback(0);
    return true;
}

void microsSofttimer_deleteTimer(uint8_t *timerKey)
{
		if(timerKey == NULL)
			return;
    callStackInDeleteTimer = true;
    sTimerList **ppListOfTimer = &timers;
    bool timerDetected = true;
    while((*ppListOfTimer)->timer.pTimerKey != timerKey)
    {
        ppListOfTimer = (sTimerList**)&((*ppListOfTimer)->next);
        if((*ppListOfTimer) == NULL) // can not detect in normal timer list
        {
            timerDetected = false;
            break;
        }
    }
    if(timerDetected == false)
    {
        ppListOfTimer = &timersUseWithCallbackFunc;
        while((*ppListOfTimer)->timer.pTimerKey != timerKey)
        {
            ppListOfTimer = (sTimerList**)&((*ppListOfTimer)->next);
            if((*ppListOfTimer) == NULL) // can not detect in callback timer list
            {
                callStackInDeleteTimer = false;
                if(reCallCallbackTimer)
                    microsSofttimer_mcuTimerCallback(0);
                return;
            }
        }
    }
    sTimerList **ppNextListOfTimer = (sTimerList**)&((*ppListOfTimer)->next);
	if((*ppListOfTimer)->timer.callbackType == TIMER_CALLBACK_TYPE_TASK ||
		(*ppListOfTimer)->timer.callbackType == TIMER_CALLBACK_TYPE_EVENT)
			free((*ppListOfTimer)->timer.signalGeneral.signalStruct);
    free((*ppListOfTimer)->timer.pTimerKey);
    free(*ppListOfTimer);
    *ppListOfTimer = *ppNextListOfTimer;
    callStackInDeleteTimer = false;
    if(reCallCallbackTimer)
        microsSofttimer_mcuTimerCallback(0);
}

void controlTimeout(sTimerList *timerList)
{
    sTimerList *pListOfTimer = timerList;
    while(pListOfTimer != NULL)
    {
        if(pListOfTimer->timer.runState == false)
        {
            pListOfTimer = (sTimerList*)pListOfTimer->next;
            continue;
        }
        uint32_t elapsedTime;
        if(pListOfTimer->timer.startingTime > timeCounter)
            elapsedTime = (0xFFFFFFFF - pListOfTimer->timer.startingTime) + timeCounter;
        else
            elapsedTime = timeCounter - pListOfTimer->timer.startingTime;
        if(elapsedTime >= pListOfTimer->timer.interval)
        {
            if(pListOfTimer->timer.callbackType == TIMER_CALLBACK_TYPE_CALLBACK_FUNC)
            {
                pListOfTimer->timer.pfnTimeoutCallback(); // run callback function of timer
                if(pListOfTimer->timer.timerType == MICROS_TIMER_TYPE_PERIODIC) // restart timer
                    pListOfTimer->timer.startingTime = timeCounter;
                else // delete in timersUseWithCallbackFunc list
                {
                    uint8_t *timerKey = pListOfTimer->timer.pTimerKey;
                    pListOfTimer = (sTimerList*)pListOfTimer->next;
                    microsSofttimer_deleteTimer(timerKey);
                    continue;
                }
            }
            pListOfTimer->timer.timeoutFlag = true; // set timeout flag
        }
        pListOfTimer = (sTimerList*)pListOfTimer->next;
    }
}

void microsSofttimer_mcuTimerCallback(uint16_t callbackPeriod)
{
    timeCounter += callbackPeriod;
    if(callStackInCreateTimer || callStackInDeleteTimer)
    {
        reCallCallbackTimer = true;
        return;
    }
    reCallCallbackTimer = false;
    controlTimeout(timersUseWithCallbackFunc);
    controlTimeout(timers);
}

sTimerList *getTimerList(void)
{
    return timers;
}

uint32_t getTimeCounterValue(void)
{
    return timeCounter;
}
