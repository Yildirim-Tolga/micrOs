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
#include "stm32l0xx_hal.h"

#ifdef MICROS_MCU_TIMER_TYPE_INTERRUPT
/**
 * @brief Timer peripheral initialization function
 * 
 */
static void micros_mcu_timer_init_periph(void);
#endif // MICROS_MCU_TIMER_TYPE_INTERRUPT

void (*pfnCb)(void);

TIM_HandleTypeDef mcuTimer;

void micros_mcu_timer_init(void (*pfnCallback)(void))
{
    pfnCb = pfnCallback;
#ifdef MICROS_MCU_TIMER_TYPE_INTERRUPT
    micros_mcu_timer_init_periph();
#endif // MICROS_MCU_TIMER_TYPE_INTERRUPT
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
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    mcuTimer.Instance = TIM2;
    mcuTimer.Init.Prescaler = 0;
    mcuTimer.Init.CounterMode = TIM_COUNTERMODE_UP;
    mcuTimer.Init.Period = 2096;
    mcuTimer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    mcuTimer.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_Base_Init(&mcuTimer) != HAL_OK)
    {
        // TODO: Error functions will be added
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&mcuTimer, &sClockSourceConfig) != HAL_OK)
    {
        // TODO: Error functions will be added
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&mcuTimer, &sMasterConfig) != HAL_OK)
    {
        // TODO: Error functions will be added
    }
    HAL_TIM_Base_Start_IT(&mcuTimer);
}

/**
 * @brief Timer callback function
 * 
 * @param htim Timer
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    pfnCb();
}

/**
 * @brief This function handles TIM2 global interrupt.
 * 
 */
void TIM2_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&mcuTimer);
}

#endif // MICROS_MCU_TIMER_TYPE_INTERRUPT
