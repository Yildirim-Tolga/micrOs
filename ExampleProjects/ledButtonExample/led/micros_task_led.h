/**
 * @file micros_task_led.h
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
#ifndef MICROS_TASK_LED_H_
#define MICROS_TASK_LED_H_

#include "micros_tse_config.h"

void led_main(void);
void led_init(void);
void led_get_signal(sSig_gen*);

#endif//MICROS_TASK_LED_H_
