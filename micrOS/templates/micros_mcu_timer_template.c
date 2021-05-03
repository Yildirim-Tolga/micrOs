/**
 * @file micros_mcu_timer.c
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

#include "micros_mcu_timer.h"
#include "micros_config.h"

#ifdef MICROS_MCU_TIMER_TYPE_INTERRUPT
/**
 * @brief Timer peripheral initialization function
 * 
 */
static void micros_mcu_timer_init_periph(void);
#endif // MICROS_MCU_TIMER_TYPE_INTERRUPT

void (*pfnCb)(void);

void micros_mcu_timer_init(void (*pfnCallback)(void))
{
    #ifdef MICROS_MCU_TIMER_TYPE_INTERRUPT
    micros_mcu_timer_init_periph();
    #endif // MICROS_MCU_TIMER_TYPE_INTERRUPT
    pfnCb = pfnCallback;
}

#ifdef MICROS_MCU_TIMER_TYPE_SYS_TIME_COUNTER
void micros_mcu_timer_main(void)
{
    // TODO: control system counter and if changed run pfnCb();
}
#endif // MICROS_MCU_TIMER_TYPE_SYS_TIME_COUNTER

#ifdef MICROS_MCU_TIMER_TYPE_INTERRUPT
static void micros_mcu_timer_init_periph(void)
{
    // TODO: create timer peripheral will be added
}

// TODO: timer interrupt callback function will be added. Restart timer and run pfnCb();
void mcu_timer_fake_interrupt(void);
void mcu_timer_fake_interrupt(void)
{
	pfnCb();
}

#endif // MICROS_MCU_TIMER_TYPE_INTERRUPT

