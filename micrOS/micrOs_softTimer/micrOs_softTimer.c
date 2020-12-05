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

bool microsSofttimer_createTimer(sTimerList *pListOfTimer)
{
    while(pListOfTimer->timer != NULL)
    {
        pListOfTimer = pListOfTimer->next;
    }
    pListOfTimer->timer = malloc(sizeof(sMicrOs_Timer));
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
    //pListOfTimer->timer->startingTime = getCurrentTimeFunction will be add
    return true;
}

void microsSofttimer_deleteTimer(sTimerList *pListOfTimer, uint8_t *timerKey)
{
    while(pListOfTimer->timer != NULL && pListOfTimer->timer->pTimerKey != timerKey)
    {
        pListOfTimer = pListOfTimer->next;
    }
    if(pListOfTimer->timer != NULL)
    {
        free(pListOfTimer->timer->pTimerKey);
        free(pListOfTimer->timer);
    }
}