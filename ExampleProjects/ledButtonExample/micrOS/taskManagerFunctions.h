/***************************************************************************************************
 * filename     : taskManagerFunctions.h
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
#ifndef TASK_MANAGER_FUNCTIONS_H_
#define TASK_MANAGER_FUNCTIONS_H_

#include "signalEventConfig.h"
#include "signalStructures.h"
#include <stdint.h>
#include <stdbool.h>

/*
** Define Timer Types
*/
#define MICROS_TIMER_TYPE_ONE_SHOT 0
#define MICROS_TIMER_TYPE_PERIODIC 1

void micrOs_setTaskRunState(eTaskId enTaskId, bool bRunState);
bool micrOs_getTaskRunState(eTaskId enTaskId);
void micrOs_dispatchEventToTask(const sSignalGeneral* signalGeneral, eTaskId enTaskId);
void micrOs_publishEventToSubscribers(eEventId enEventID, const sSignalGeneral* signalGeneral);		// publish-Subscribers
void micrOs_setEventSubscriptionState(eEventId enEventID, eTaskId enTaskId, bool bSubscribeState);

uint8_t *micrOs_startEventPublishTimer(bool bTimerType, uint32_t dwInterval, eEventId enEventID, const sSignalGeneral* signalGeneral);
uint8_t *micrOs_startEventDispachTimer(bool bTimerType, uint32_t dwInterval, eTaskId enTaskId, const sSignalGeneral* signalGeneral);
uint8_t *micrOs_startCallbackTimer(bool bTimerType, uint32_t dwInterval, void (*pfnCallbackFunc)(void));
void micrOs_cancelTimer(uint8_t *byTimerKey);

#endif //TASK_MANAGER_FUNCTIONS_H_
