/***************************************************************************************************
 * filename     : micrOsTask_led.c
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

#include "ledTask.h"
#include "micrOsTask_led.h"
#include "../micrOS/signalEventConfig.h"
#include "../micrOS/signalStructures.h"
#include <stdint.h>
#include <stddef.h>

void taskLedMain(void)
{

}

void taskLedInit(void)
{
    sSignalGeneral signal;
    signal.signalType = SIGNAL_TYPE__ONLY_ID;
    sSignalOnlyId signalStruct;
    signalStruct.signalId = SIGNAL_ID__TEST_SINGAL_2;
    signal.signalStruct = &signalStruct;
    micrOs_dispatchEventToTask(&signal,TASK_ID__LED);
}

void taskLedGetSignal(sSignalGeneral *signalVar)
{
    switch(signalVar->signalType)
    {
        case SIGNAL_TYPE__ONLY_ID:
        {
            sSignalOnlyId gettingSignal = *(sSignalOnlyId*)(signalVar->signalStruct);
            printf("Get Led signal ==> signal type code : %d , signal code : %d",signalVar->signalType,gettingSignal.signalId);
            break;
        }
        case SIGNAL_TYPE__WITH_U8_DATA:
        {
            sSignalWithU8Data gettingSignal = *(sSignalWithU8Data*)(signalVar->signalStruct);
            printf("Get Led signal ==> signal type code : %d , signal code : %d data : %d",signalVar->signalType,gettingSignal.signalId);
            break;
        }
    }
}
