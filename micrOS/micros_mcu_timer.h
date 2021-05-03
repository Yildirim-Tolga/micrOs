/**
 * @file micros_mcu_timer.h
 * @author Tolga Yildirim (tolga.yildirimmm@gmail.com)
 * @brief 
 * @version 1.0
 * @date 2021-04-30
 * 
 * @copyright
 * 
 * Version History:  +,-,/,*: add, remove, change, fix
 *  v1.0:   First release.
 */

#ifndef MICROS_MCU_TIMER_H_
#define MICROS_MCU_TIMER_H_

/**
 * @brief Microcontroller timer initialize function
 * 
 * @param pfnCallback Callback function
 */
void micros_mcu_timer_init(void (*pfnCallback)(void));

#ifdef MICROS_MCU_TIMER_TYPE_CTRL_IN_LOOP
/**
 * @brief Microcontroller time control and run cb
 * 
 */
void micros_mcu_timer_main(void);
#endif // MICROS_MCU_TIMER_TYPE_CTRL_IN_LOOP

#endif // MICROS_MCU_TIMER_H_
