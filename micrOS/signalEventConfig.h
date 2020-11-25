/***************************************************************************************************
 * filename     : signalEventConfig.h
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
#ifndef SIGNAL_EVENT_CONFIG_H_
#define SIGNAL_EVENT_CONFIG_H_

/*
** Define Task Id
*/
typedef enum eTaskId_
{
    TASK_ID__LED,
    TASK_ID__BUTTON,
    SYSTEM_TASK_COUNT
}eTaskId;

/*
** Define Event Types
*/
typedef enum eEventId_
{
    EVENT_TYPE__TEST_EVENT_1,
    EVENT_TYPE__TEST_EVENT_2,
    SYSTEM_EVENT_COUNT
}eEventId;

/*
** Define Signal Id
*/
typedef enum eSignalId_
{
    SIGNAL_ID__TEST_SINGAL_1,
    SIGNAL_ID__TEST_SINGAL_2,
    SIGNAL_ID__TEST_SINGAL_3
}eSignalId;

#endif //SIGNAL_EVENT_CONFIG_H_
