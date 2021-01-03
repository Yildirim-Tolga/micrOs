/***************************************************************************************************
 * filename     : taskManager.h
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
#ifndef TASK_MANAGER_H_
#define TASK_MANAGER_H_

#include "signalEventConfig.h"

void micrOs_allTaskInit(void);
void micrOs_taskInit(eTaskId enTaskId);
void micrOs_main(void);
void micrOs_init(void);

#endif //TASK_MANAGER_H_
