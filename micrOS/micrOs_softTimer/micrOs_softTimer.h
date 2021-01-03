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
#include "../taskManagerFunctions.h"

#define TIMER_CALLBACK_TYPE_CALLBACK_FUNC 0
#define TIMER_CALLBACK_TYPE_TASK 1
#define TIMER_CALLBACK_TYPE_EVENT 2

/* ========================================  Start of section using anonymous unions  ======================================== */
#if defined (__CC_ARM)
  #pragma push
  #pragma anon_unions
#elif defined (__ICCARM__)
  #pragma language=extended
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wc11-extensions"
  #pragma clang diagnostic ignored "-Wreserved-id-macro"
  #pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
  #pragma clang diagnostic ignored "-Wnested-anon-types"
#elif defined (__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined (__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined (__TASKING__)
  #pragma warning 586
#elif defined (__CSMC__)
  /* anonymous unions are enabled by default */
#else
  #warning Not supported compiler type
#endif
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
        };
    };
    bool timeoutFlag;
    bool runState;
}sMicrOs_Timer;
/* =========================================  End of section using anonymous unions  ========================================= */
#if defined (__CC_ARM)
  #pragma pop
#elif defined (__ICCARM__)
  /* leave anonymous unions enabled */
#elif (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic pop
#elif defined (__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined (__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined (__TASKING__)
  #pragma warning restore
#elif defined (__CSMC__)
  /* anonymous unions are enabled by default */
#endif

/*
** Timer List Structure
*/
typedef struct sTimerList_
{
    sMicrOs_Timer timer;
    struct sTimerList *next;
}sTimerList;

bool microsSofttimer_createTimer(sMicrOs_Timer **timer,uint8_t callbackType);
void microsSofttimer_deleteTimer(uint8_t *timerKey);
sTimerList *getTimerList(void);
uint32_t getTimeCounterValue(void);

void microsSofttimer_mcuTimerCallback(uint16_t callbackPeriod);

#endif //MICROS_SOFTTIMER_H_
