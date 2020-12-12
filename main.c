/***************************************************************************************************
 * filename     : main.h
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
#include <stdio.h>
#include <stdlib.h>
#include "micrOS/taskManager.h"
#include "micrOS/micrOs_softTimer/micrOs_softTimer.h"

int main()
{
    micrOs_init();
    micrOs_allTaskInit();
    uint16_t fakeMicrosecondInterruptCnt = 0;
    while(1)
    {
        micrOs_main();
        fakeMicrosecondInterruptCnt++;
        if(fakeMicrosecondInterruptCnt == 10000)
        {
            fakeMicrosecondInterruptCnt = 0;
            microsSofttimer_mcuTimerCallback(1);
        }
    }
    return 0;
}
