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
    EVENT_TYPE__LED_MODE,
    SYSTEM_EVENT_COUNT
}eEventId;

/*
** Define Signal Id
*/
typedef enum eSignalId_
{
    SIGNAL_ID__LED_MODE_CHANGED,
		SIGNAL_ID__SELECTED_LED_CHANGED,
		SIGNAL_ID__DEBOUNCE_TIMEOUT,
		SIGNAL_ID__LONG_PRESS_TIMEOUT,
		SIGNAL_ID__LED_SEL_TIMEOUT,
		SIGNAL_ID__LED1_TIMEOUT,
		SIGNAL_ID__LED2_TIMEOUT,
		SIGNAL_ID__LED3_TIMEOUT
}eSignalId;

#endif //SIGNAL_EVENT_CONFIG_H_
