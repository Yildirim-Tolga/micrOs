/***************************************************************************************************
 * filename     : micrOs_softTimer.h
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

#ifndef MICROS_SOFTTIMER_H_
#define MICROS_SOFTTIMER_H_

#include <stdint.h>
#include <stdbool.h>
#include "../signalStructures.h"

/*
** Define Timer Types
*/
#define MICROS_TIMER_TYPE_ONE_SHOT 0
#define MICROS_TIMER_TYPE_PERIODIC 1



#define TIMER_CALLBACK_TYPE_CALLBACK_FUNC 0
#define TIMER_CALLBACK_TYPE_TASK 1
#define TIMER_CALLBACK_TYPE_EVENT 2

/*
** Timer Structure
*/
typedef struct sMicrOs_Timer_
{
    uint8_t *pTimerKey; // timer key pointer
    uint32_t startingTime;
    uint32_t interval; // in ms
    bool timerType; // one shot or periodic timer

    // use for timer output type -use callback function, send signal to task or send signal to subscribers-
    uint8_t callbackType:3;
    union
    {
        void (*pfnTimeoutCallback)(void);
        struct{
            union{
                eEventId event;
                eTaskId task;
            };
            sSignalGeneral signalGeneral;
            bool timeoutFlag;
        };
    };
    bool runState;
}sMicrOs_Timer;

/*
** Timer List Structure
*/
typedef struct sTimerList_
{
    sMicrOs_Timer *timer;
    struct sTimerList *next;
}sTimerList;

bool microsSofttimer_createTimer(sMicrOs_Timer *timer);
void microsSofttimer_deleteTimer(uint8_t *timerKey);

#endif //MICROS_SOFTTIMER_H_
