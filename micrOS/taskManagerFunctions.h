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
#include "signalEventConfig.h"
#include "signalStructures.h"
#include <stdint.h>
#include <stdbool.h>

void micrOs_setTaskRunState(eTaskId enTaskId, bool bRunState);
bool micrOs_getTaskRunState(eTaskId enTaskId);
void micrOs_dispatchEventToTask(const sSignalGeneral* signalGeneral, eTaskId enTaskId);
void micrOs_publishEventToSubscribers(eEventId enEventID, const sSignalGeneral* signalGeneral);		// publish-Subscribers
void micrOs_setEventSubscriptionState(eEventId enEventID, eTaskId enTaskId, bool bSubscribeState);

uint8_t *micrOs_startEventPublishTimer(bool bTimerType, uint32_t dwInterval, eEventId enEventID, const sSignalGeneral* signalGeneral);
uint8_t *micrOs_startEventDispachTimer(bool bTimerType, uint32_t dwInterval, eTaskId enTaskId, const sSignalGeneral* signalGeneral);
void micrOs_cancelTimer(uint8_t *byTimerKey);
