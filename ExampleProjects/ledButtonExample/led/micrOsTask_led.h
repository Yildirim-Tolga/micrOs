/***************************************************************************************************
 * filename     : micrOsTask_led.h
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
#ifndef MICROS_TASK_LED_H_
#define MICROS_TASK_LED_H_

#include "../micrOS/signalStructures.h"

void taskLedMain(void);
void taskLedInit(void);
void taskLedGetSignal(sSignalGeneral*);

#endif//MICROS_TASK_LED_H_
