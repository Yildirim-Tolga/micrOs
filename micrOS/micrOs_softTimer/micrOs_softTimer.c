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
static sTimerList timers;

bool microsSofttimer_createTimer(sMicrOs_Timer *timer)
{
    sTimerList *pListOfTimer = &timers;
    while(pListOfTimer->timer != NULL)
    {
        pListOfTimer = (sTimerList*)pListOfTimer->next;
    }
    pListOfTimer->timer = malloc(sizeof(sMicrOs_Timer));
    pListOfTimer->timer->runState = false;
    if(pListOfTimer->timer == NULL)
    {
        errorHandler(ERR_CODE_MALLOC_TIMER_LIST);
        return false;
    }
    pListOfTimer->timer->pTimerKey = malloc(sizeof(uint8_t));
    if(pListOfTimer->timer->pTimerKey)
    {
        errorHandler(ERR_CODE_MALLOC_TIMER_KEY);
        return false;
    }
    pListOfTimer->timer->startingTime = timeCounter;
    timer = pListOfTimer->timer;
    return true;
}

void microsSofttimer_deleteTimer(uint8_t *timerKey)
{
    sTimerList *pListOfTimer = &timers;
    while(pListOfTimer->timer != NULL && pListOfTimer->timer->pTimerKey != timerKey)
    {
        pListOfTimer = (sTimerList*)pListOfTimer->next;
    }
    if(pListOfTimer->timer != NULL)
    {
        free(pListOfTimer->timer->pTimerKey);
        free(pListOfTimer->timer);
    }
}

void controlTimeout(void)
{
    sTimerList *pListOfTimer = &timers;
    while(pListOfTimer->timer != NULL)
    {
        if(pListOfTimer->timer->runState == false)
        {
            pListOfTimer = (sTimerList*)pListOfTimer->next;
            continue;
        }
        uint32_t elapsedTime;
        if(pListOfTimer->timer->startingTime > timeCounter)
            elapsedTime = (0xFFFFFFFF - pListOfTimer->timer->startingTime) + timeCounter;
        else
            elapsedTime = timeCounter - pListOfTimer->timer->startingTime;
        if(elapsedTime >= pListOfTimer->timer->interval)
        {
            if(pListOfTimer->timer->callbackType == TIMER_CALLBACK_TYPE_CALLBACK_FUNC)
                pListOfTimer->timer->pfnTimeoutCallback(); // run callback function of timer
            else
                pListOfTimer->timer->timeoutFlag = true; // set timeout flag
        }
        pListOfTimer = (sTimerList*)pListOfTimer->next;
    }
}
