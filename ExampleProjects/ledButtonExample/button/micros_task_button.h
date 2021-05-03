/**
 * @file micros_task_button.h
 * @author Tolga Yildirim (tolga.yildirimmm@gmail.com)
 * @brief 
 * @version 1.0
 * @date 2021-05-03
 * 
 * @copyright
 * 
 * Version History:  +,-,/,*: add, remove, change, fix
 *  v1.0:   First release.
 */
#ifndef MICROS_TASK_BUTTON_H_
#define MICROS_TASK_BUTTON_H_

#include "micros_tse_config.h"

void button_main(void);
void button_init(void);
void button_get_signal(sSig_gen *);

#endif //MICROS_TASK_BUTTON_H_
