/**
 * @file micros.h
 * @author Tolga Yildirim (tolga.yildirimmm@gmail.com)
 * @brief micrOS functions for task usage
 * @version 1.0
 * @date 2021-04-29
 * 
 * @copyright
 * 
 * Version History:  +,-,/,*: add, remove, change, fix
 *  v1.0:   First release.
 */

#ifndef MICROS_H_
#define MICROS_H_

#include "micros_tse_config.h"
#include "generic_def.h"

#define MICROS_TM_TYPE_ONE_SHOT 0
#define MICROS_TM_TYPE_PERIODIC 1

/**
 * @brief Set run state of task with task id
 * 
 * @param task_id Task ID
 * @param run_state Run state true or false
 */
void micros_task_runstate_set(eTaskId task_id, uint8_t run_state);

/**
 * @brief Get task state
 * 
 * @param task_id Task ID
 * @return true Run
 * @return false Stop
 */
uint8_t micros_task_runstate_get(eTaskId task_id);

/**
 * @brief Send signal to task with task ID
 * 
 * @param sig_gen Signal with signal general structure
 * @param task_id Task ID
 */
void micros_event_dispatch(const sSig_gen *sig_gen, eTaskId task_id);

/**
 * @brief Send signal to subscribers
 * 
 * @param event_id Event ID
 * @param sig_gen Signal with signal general structure
 */
void micros_event_publish(eEventId event_id, const sSig_gen *sig_gen);

/**
 * @brief Set subscription state to event
 * 
 * @param event_id Event ID
 * @param task_id Task ID
 * @param sub_state Subscription state true: Subscripe false: Unsubscripe
 */
void micros_event_substate_set(eEventId event_id, eTaskId task_id, uint8_t sub_state);

/**
 * @brief Publish signal to tasks which in the subscribe list for event id after interval time
 * 
 * @param tm_type MICROS_TM_TYPE_ONE_SHOT or MICROS_TM_TYPE_PERIODIC
 * @param cancelable true=cancelable. If cancelable event this function return timer_key else return 0.
 * @param interval Timer interval
 * @param event_id Event ID
 * @param sig_gen Signal with signal general structure
 * @return uint8_t If cancelable event this function return timer_key else return 0.
 */
uint8_t micros_timer_event_publish(uint8_t tm_type, uint8_t cancelable, uint32_t interval, eEventId event_id, const sSig_gen *sig_gen);

/**
 * @brief Dispatch signal to task after interval time
 * 
 * @param tm_type MICROS_TM_TYPE_ONE_SHOT or MICROS_TM_TYPE_PERIODIC
 * @param cancelable true=cancelable. If cancelable event this function return timer_key else return 0.
 * @param interval Timer interval
 * @param task_id Task ID
 * @param sig_gen Signal with signal general structure
 * @return uint8_t If cancelable event this function return timer_key else return 0.
 */
uint8_t micros_timer_event_dispatch(uint8_t tm_type, uint8_t cancelable, uint32_t interval, eTaskId task_id, const sSig_gen *sig_gen);

/**
 * @brief Start timer and end of the interval time run callback function
 * 
 * @param tm_type MICROS_TM_TYPE_ONE_SHOT or MICROS_TM_TYPE_PERIODIC
 * @param cancelable true=cancelable. If cancelable event this function return timer_key else return 0.
 * @param interval Timer interval
 * @param pfnCallbackFunc Callback function
 * @return uint8_t If cancelable event this function return timer_key else return 0.
 */
uint8_t micros_timer_start(uint8_t tm_type, uint8_t cancelable, uint32_t interval, void (*pfnCallbackFunc)(void));

/**
 * @brief Cancel timer
 * 
 * @param tm_key Timer key
 */
void micros_timer_cancel(uint8_t tm_key);

#endif //MICROS_H_
