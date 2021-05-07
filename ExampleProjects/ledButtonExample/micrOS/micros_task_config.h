/**
 * @file micros_task_config.h
 * @author Tolga Yildirim (tolga.yildirimmm@gmail.com)
 * @brief micrOS configuration file
 * @version 1.0
 * @date 2021-04-29
 * 
 * @copyright
 * 
 * Version History:  +,-,/,*: add, remove, change, fix
 *  v1.0:   First release.
 */

#ifndef MICROS_TASK_CONFIG_
#define MICROS_TASK_CONFIG_

#include <stdlib.h>
#include "micros_tse_config.h"
#include "micros_config.h"
#include "generic_def.h"

#include "micros_task_button.h"
#include "micros_task_led.h"

/**
 * @brief Linked list node structure for signal queue
 * 
 */
typedef struct sSig_node
{
    sSig_gen sig_gen;
    struct sSig_node *next;
} sSig_node;

/**
 * @brief Task structure
 * 
 */
typedef struct sMicrOs_task
{
    void (*const pfnInitTask)(void);
    void (*const pfnMainTask)(void);
    void (*const pfnGetSignal)(sSig_gen *);
    uint8_t run_state : 1;
    sSig_node *pSig_head;
    sSig_node *pSig_tail;
} sMicrOs_task;

/**
 * @brief Task functions and state initialize (In the order at eTaskId)
 * 
 */
static sMicrOs_task micros_task[SYSTEM_TASK_COUNT] =
    {
        {.pfnInitTask = led_init,
         .pfnMainTask = led_main,
         .pfnGetSignal = led_get_signal,
         .run_state = TASK_STATE_RUN},
        {.pfnInitTask = button_init,
         .pfnMainTask = button_main,
         .pfnGetSignal = button_get_signal,
         .run_state = TASK_STATE_RUN}};

/**
 * @brief Set signal size variable (In the order at eSignalType)
 * 
 */
const size_t sig_size[SIGNAL_TYPE__COUNT] = {sizeof(sSig_id), sizeof(sSig_id_u8)};

#endif //MICROS_CONFIG_H_
