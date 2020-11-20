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

int main()
{
    micrOs_allTaskInit();
    while(1)
    {
        micrOs_main();
    }
    return 0;
}