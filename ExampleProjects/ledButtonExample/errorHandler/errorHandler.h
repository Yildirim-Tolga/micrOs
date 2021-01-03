/***************************************************************************************************
 * filename     : errorHandler.h
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

#ifndef ERROR_HANDLER_H_
#define ERROR_HANDLER_H_

typedef enum eErrorCode_
{
    ERR_CODE_MALLOC_SIGNAL_LIST,
    ERR_CODE_MALLOC_SIGNAL_STRUCT,
    ERR_CODE_MALLOC_SUBSCRIBE_LIST,
    ERR_CODE_MALLOC_TIMER_LIST,
    ERR_CODE_MALLOC_TIMER_KEY,
		ERR_CODE_TIMER_PERIPHERAL_INIT
    /*
    ...
    ...
    */
}eErrorCode;

void errorHandler(eErrorCode errId);


#endif //ERROR_HANDLER_H_

