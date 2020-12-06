/***************************************************************************************************
 * filename     : taskManager.c
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

#include "taskManagerConfig.h"
#include "taskManager.h"
#include "taskManagerFunctions.h"
#include "../errorHandler/errorHandler.h"
#include "micrOs_softTimer/micrOs_softTimer.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/*
** Structure of Event Subscribe List
*/
typedef struct sEventSubscribeList_
{
    eTaskId taskId;
    struct sEventSubscribeList *next;
}sEventSubscribeList;

static sEventSubscribeList *eventSignal[SYSTEM_EVENT_COUNT];

static bool getSignal(eTaskId taskId,sSignalGeneral *signal);
static void setSignal(eTaskId taskId,sSignalGeneral *signal);

static void addTaskSubscribeList(eEventId enEventID, eTaskId enTaskId);
static void deleteTaskSubscribeList(eEventId enEventID, eTaskId enTaskId);


void micrOs_allTaskInit(void)
{
    for(uint8_t taskIdCounter = 0; taskIdCounter < SYSTEM_TASK_COUNT; taskIdCounter++)
        micrOsTask[taskIdCounter].pfnInitTask();
}

void micrOs_taskInit(eTaskId enTaskId)
{
    micrOsTask[enTaskId].pfnInitTask();
}

void micrOs_main(void)
{
    for(uint8_t taskIdCounter = 0; taskIdCounter < SYSTEM_TASK_COUNT; taskIdCounter++)
    {
        if(!micrOsTask[taskIdCounter].bTaskStartUpState) //control run state
            return;
        sSignalGeneral gettingSignal;
        while(getSignal(taskIdCounter,&gettingSignal))// if signal not received go to main function directly, else run get signal function.
        {
            // run task get signal function
            micrOsTask[taskIdCounter].pfnGetSignal(&gettingSignal);
            // delete allocated memory for signal
            free(gettingSignal.signalStruct);
            // get signal head address for delete
            sSignalList **ppSignalListHead = &(micrOsTask[taskIdCounter].pSignalListHead);
            // set head address to next signal address
            sSignalList *pSignalListNext = (sSignalList*)micrOsTask[taskIdCounter].pSignalListHead->next;
            // delete head list
            free(*ppSignalListHead);
            // set signal list head
            micrOsTask[taskIdCounter].pSignalListHead = pSignalListNext;
        }
        micrOsTask[taskIdCounter].pfnMainTask();
    }
}

static bool getSignal(eTaskId taskId,sSignalGeneral *signal)
{
    if(micrOsTask[taskId].pSignalListHead == NULL)
        return false;
    else
    {
        signal->signalType = micrOsTask[taskId].pSignalListHead->signalGeneral.signalType;
        signal->signalStruct = micrOsTask[taskId].pSignalListHead->signalGeneral.signalStruct;
        return true;
    }
}

static void setSignal(eTaskId taskId,sSignalGeneral *signal)
{
    sSignalList **ppAddingSignalList = &(micrOsTask[taskId].pSignalListHead);;
    while((*ppAddingSignalList) != NULL)
    {
        ppAddingSignalList = (sSignalList**)&(*ppAddingSignalList)->next;
    }
    // allocate memory for new signal list
    *ppAddingSignalList = malloc(sizeof(sSignalList));
    // control memory allocation
    if(*ppAddingSignalList == NULL)
    {
        errorHandler(ERR_CODE_MALLOC_SIGNAL_LIST);
        return;
    }
    // get signal type
    (*ppAddingSignalList)->signalGeneral.signalType = signal->signalType;
    // allocate memory for signal structure
    (*ppAddingSignalList)->signalGeneral.signalStruct = malloc(structSize[signal->signalType]);
    // control memory allocation
    if((*ppAddingSignalList) == NULL)
    {
        free((*ppAddingSignalList));
        errorHandler(ERR_CODE_MALLOC_SIGNAL_STRUCT);
        return;
    }
    // get signal variables
    memcpy((*ppAddingSignalList)->signalGeneral.signalStruct,signal->signalStruct,structSize[signal->signalType]);
}

void micrOs_setTaskRunState(eTaskId enTaskId, bool bRunState)
{
    if(enTaskId >= SYSTEM_TASK_COUNT) // unknown task id
        return;
    micrOsTask[enTaskId].bTaskStartUpState = bRunState;
}

bool micrOs_getTaskRunState(eTaskId enTaskId)
{
    if(enTaskId >= SYSTEM_TASK_COUNT) // unknown task id
        return false;
    return micrOsTask[enTaskId].bTaskStartUpState;
}

void micrOs_dispatchEventToTask(const sSignalGeneral* signalGeneral, eTaskId enTaskId)
{
    if(enTaskId >= SYSTEM_TASK_COUNT) // unknown task id
        return;
    if(!micrOsTask[enTaskId].bTaskStartUpState) //control run state
        return;
    setSignal(enTaskId,(sSignalGeneral*)signalGeneral);
}

void micrOs_publishEventToSubscribers(eEventId enEventID, const sSignalGeneral* signalGeneral)		// publish-Subscribers
{
    if(enEventID >= SYSTEM_EVENT_COUNT) // unknown event id
        return;
    sEventSubscribeList *currentEventSubList = eventSignal[enEventID];
    while( currentEventSubList != NULL)
    {
        micrOs_dispatchEventToTask(signalGeneral,currentEventSubList->taskId);
        currentEventSubList = (sEventSubscribeList*)currentEventSubList->next;
    }
}

void micrOs_setEventSubscriptionState(eEventId enEventID, eTaskId enTaskId, bool bSubscribeState)
{
    if(enEventID >= SYSTEM_EVENT_COUNT || enTaskId >= SYSTEM_TASK_COUNT) // unknown event id or task id
        return;
    if(bSubscribeState)
        addTaskSubscribeList(enEventID,enTaskId);
    else
        deleteTaskSubscribeList(enEventID,enTaskId);
}

static void addTaskSubscribeList(eEventId enEventID, eTaskId enTaskId)
{
    sEventSubscribeList **ppAddingSubscribeList = &eventSignal[enEventID];
    while((*ppAddingSubscribeList) != NULL)
    {
        if((*ppAddingSubscribeList)->taskId == enTaskId)// already added list
            return;
        ppAddingSubscribeList = (sEventSubscribeList**)&(*ppAddingSubscribeList)->next;
    }
    // allocate memory for new subscribe list
    *ppAddingSubscribeList = malloc(sizeof(sEventSubscribeList));
    // control memory allocation
    if(*ppAddingSubscribeList == NULL)
    {
        errorHandler(ERR_CODE_MALLOC_SUBSCRIBE_LIST);
        return;
    }
    // set task id
    (*ppAddingSubscribeList)->taskId = enTaskId;
}

static void deleteTaskSubscribeList(eEventId enEventID, eTaskId enTaskId)
{
    sEventSubscribeList **ppCurrentSubscribeList = &eventSignal[enEventID];
    sEventSubscribeList **ppPrevSubscribeList;
    if((*ppCurrentSubscribeList) == NULL)// not detect this task id in list
        return;
    while((*ppCurrentSubscribeList)->taskId != enTaskId)
    {
        ppPrevSubscribeList = ppCurrentSubscribeList;
        (*ppCurrentSubscribeList) = (sEventSubscribeList*)(*ppCurrentSubscribeList)->next;
        if((*ppCurrentSubscribeList) == NULL)// not detect this task id in list
            return;
    }
    (*ppPrevSubscribeList)->next = (*ppCurrentSubscribeList)->next;
    free((*ppCurrentSubscribeList));
}

uint8_t *micrOs_startEventPublishTimer(bool bTimerType, uint32_t dwInterval, eEventId enEventID, const sSignalGeneral* signalGeneral)
{
    sMicrOs_Timer tmpTimer;
    tmpTimer.callbackType = TIMER_CALLBACK_TYPE_EVENT;
    sMicrOs_Timer *timer = &tmpTimer;
    if(microsSofttimer_createTimer(timer))
        return NULL;
    timer->event = enEventID;
    timer->interval = dwInterval;
    timer->signalGeneral.signalType = signalGeneral->signalType;
    memcpy(timer->signalGeneral.signalStruct,signalGeneral->signalStruct,structSize[signalGeneral->signalType]);
    timer->timeoutFlag = false;
    timer->timerType = bTimerType;
    timer->callbackType = TIMER_CALLBACK_TYPE_EVENT;
    timer->runState = true;
    return timer->pTimerKey;
}

uint8_t *micrOs_startEventDispachTimer(bool bTimerType, uint32_t dwInterval, eTaskId enTaskId, const sSignalGeneral* signalGeneral)
{
    sMicrOs_Timer tmpTimer;
    tmpTimer.callbackType = TIMER_CALLBACK_TYPE_TASK;
    sMicrOs_Timer *timer = &tmpTimer;
    if(microsSofttimer_createTimer(timer))
        return NULL;
    timer->event = enTaskId;
    timer->interval = dwInterval;
    timer->signalGeneral.signalType = signalGeneral->signalType;
    timer->signalGeneral.signalStruct = malloc(structSize[timer->signalGeneral.signalType]);
    memcpy(timer->signalGeneral.signalStruct,signalGeneral->signalStruct,structSize[signalGeneral->signalType]);
    timer->timeoutFlag = false;
    timer->timerType = bTimerType;
    timer->callbackType = TIMER_CALLBACK_TYPE_TASK;
    timer->runState = true;
    return timer->pTimerKey;
}

void micrOs_cancelTimer(uint8_t *byTimerKey)
{
    microsSofttimer_deleteTimer(byTimerKey);
}
