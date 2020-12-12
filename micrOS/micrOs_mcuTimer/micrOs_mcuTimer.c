/***************************************************************************************************
 * filename     : micrOs_mcuTimer.c
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

#include "micrOs_mcuTimer.h"
#include "../micrOs_softTimer/micrOs_softTimer.h"

#define MCU_TIMER_PERIOD 1 //ms

void microsMcuTimer_init(void)
{
    /*
        initialize timer peripheral
        enable global interrupt
        enable timer interrupt
        start timer
    */
}

// timer callback function will be added
/*  
    clear timeout flag
    restart timer
    run microsSofttimer_mcuTimerCallback function
*/