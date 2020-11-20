/***************************************************************************************************
 * filename     : taskManagerConfig.h
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
#ifndef TASK_MANAGER_CONFIG_H_
#define TASK_MANAGER_CONFIG_H_

#include "signalStructures.h"

/*
** Should be include task header files
**
** Example:
** #include "micrOsTask_Led.h"
*/

#include "../button/micrOsTask_button.h"
#include "../led/micrOsTask_led.h"
#include <stdbool.h>

#define TASK_STATE_RUN true
#define TASK_STATE_STOP false

/*
** Structure of Signal List
*/
typedef struct sSignalList_
{
    sSignalGeneral signalGeneral;
    struct sSignalList *next;
}sSignalList;

/*
** Define Task Structure
*/
typedef struct sMicrosTask_
{
    void const (*pfnInitTask)(void);
    void const (*pfnMainTask)(void);
    void const (*pfnGetSignal)(sSignalGeneral*);
    bool bTaskStartUpState;
    sSignalList *pSignalListHead;
}sMicrOsTask;

static sMicrOsTask micrOsTask[SYSTEM_TASK_COUNT] =
{
    {
        .pfnInitTask = taskLedInit,
        .pfnMainTask = taskLedMain,
        .pfnGetSignal = taskLedGetSignal,
        .bTaskStartUpState = TASK_STATE_RUN
    },
    {
        .pfnInitTask = taskButtonInit,
        .pfnMainTask = taskButtonMain,
        .pfnGetSignal = taskButtonGetSignal,
        .bTaskStartUpState = TASK_STATE_RUN
    }
};


#endif //TASK_MANAGER_CONFIG_H_
