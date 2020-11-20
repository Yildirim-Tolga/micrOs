/***************************************************************************************************
 * filename     : signalStructures.h
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
#ifndef TASK_MANAGER_STRUCT_H_
#define TASK_MANAGER_STRUCT_H_

#define TASK_STATE_RUN true
#define TASK_STATE_STOP false

#include <stdint.h>
#include "signalEventConfig.h"

/*
** Define Signal Types
*/
#define SIGNAL_TYPE__COUNT 2
#if SIGNAL_TYPE__COUNT > 1
typedef enum eSignalTypes_
{
    SIGNAL_TYPE__ONLY_ID,
    SIGNAL_TYPE__WITH_U8_DATA
}eSignalTypes;

typedef struct sSignalOnlyId_
{
    eSignalId signalId;
}sSignalOnlyId;

typedef struct sSignalWithU8Data_
{
    eSignalId signalId;
    uint8_t data;
}sSignalWithU8Data;

/*
** Define General Signal Type
*/
typedef struct sSignalGeneral_
{
    eSignalTypes signalType;
    void *signalStruct;
}sSignalGeneral; 
#else
typedef struct sSignalGeneral_
{
    eSignalId *signalStruct;
    // if you need, add extra parameter
}sSignalGeneral;
#endif //SIGNAL_TYPE__COUNT > 1

#endif //TASK_MANAGER_STRUCT_H_
