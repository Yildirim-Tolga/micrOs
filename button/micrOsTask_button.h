/***************************************************************************************************
 * filename     : micrOsTask_button.h
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
#ifndef MICROS_TASK_BUTTON_H_
#define MICROS_TASK_BUTTON_H_

#include "../micrOS/signalStructures.h"

void taskButtonMain(void);
void taskButtonInit(void);
void taskButtonGetSignal(sSignalGeneral*);

#endif//MICROS_TASK_BUTTON_H_
