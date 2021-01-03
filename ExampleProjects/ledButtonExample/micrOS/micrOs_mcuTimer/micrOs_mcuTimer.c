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
#include "micrOs_softTimer.h"
#include "stm32l0xx_hal.h"
#include "errorHandler.h"

#define MCU_TIMER_PERIOD 1 //ms

TIM_HandleTypeDef mcuTimer;

void microsMcuTimer_init(void)
{
    /*
        initialize timer peripheral
        start timer interrupt
    */
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
    errorHandler(ERR_CODE_TIMER_PERIPHERAL_INIT);
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&mcuTimer, &sClockSourceConfig) != HAL_OK)
  {
    errorHandler(ERR_CODE_TIMER_PERIPHERAL_INIT);
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&mcuTimer, &sMasterConfig) != HAL_OK)
  {
    errorHandler(ERR_CODE_TIMER_PERIPHERAL_INIT);
  }
	HAL_TIM_Base_Start_IT(&mcuTimer);
}

// timer callback function will be added
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	microsSofttimer_mcuTimerCallback(MCU_TIMER_PERIOD);
}

/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM2_IRQHandler(void)
{
  /* USER CODE BEGIN TIM2_IRQn 0 */

  /* USER CODE END TIM2_IRQn 0 */
  HAL_TIM_IRQHandler(&mcuTimer);
  /* USER CODE BEGIN TIM2_IRQn 1 */

  /* USER CODE END TIM2_IRQn 1 */
}
