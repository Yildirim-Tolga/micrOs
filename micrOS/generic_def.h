/**
 * @file generic_def.c
 * @author Tolga Yildirim (tolga.yildirimmm@gmail.com)
 * @brief 
 * @version 1.0
 * @date 2021-05-07
 * 
 * @copyright
 * 
 * Version History:  +,-,/,*: add, remove, change, fix
 *  v1.0:   First release.
 */

#ifndef GENERIC_DEF_H_
#define GENERIC_DEF_H_

#ifndef TRUE
#define TRUE 1
#else
#warning "TRUE pre-defined another module to micros, please control TRUE == 1"
#endif // TRUE

#ifndef FALSE
#define FALSE 0
#else
#warning "FALSE pre-defined another module to micros, please control FALSE == 0"
#endif // FALSE

#endif // GENERIC_DEF_H_
