/**
 * @file micros_core.h
 * @author Tolga Yildirim (tolga.yildirimmm@gmail.com)
 * @brief 
 * @version 1.0
 * @date 2021-04-29
 * 
 * @copyright
 * 
 * Version History:  +,-,/,*: add, remove, change, fix
 *  v1.0:   First release.
 */

#ifndef MICROS_CORE_H_
#define MICROS_CORE_H_

#include "micros_tse_config.h"

/**
 * @brief All tasks initialize function. (In the order at eTaskId)
 * 
 */
void micros_allTaskInit(void);

/**
 * @brief Only one task initialize function with task ID
 * 
 * @param task_id 
 */
void micros_taskInit(eTaskId task_id);

/**
 * @brief micros main function should be run in main while
 * 
 */
void micros_main(void);

/**
 * @brief micros initialize function must be run one time at mcu start running
 * 
 */
void micros_init(void);

#endif //MICROS_CORE_H_
