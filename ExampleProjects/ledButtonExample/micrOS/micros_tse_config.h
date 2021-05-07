/**
 * @file micros_se_config.h
 * @author Tolga Yildirim (tolga.yildirimmm@gmail.com)
 * @brief There are micrOS tasks, signals and event id definitions. Signal structures will be declareted.
 * @version 1.0
 * @date 2021-04-29
 * 
 * @copyright
 * 
 * Version History:  +,-,/,*: add, remove, change, fix
 *  v1.0:   First release.
 */

#ifndef MICROS_TSE_CONFIG_H_
#define MICROS_TSE_CONFIG_H_

#include <stdint.h>

/**
 * @brief Task IDs
 * 
 */
typedef enum eTaskId
{
    TASK_ID__LED,
    TASK_ID__BUTTON,
    SYSTEM_TASK_COUNT
} eTaskId;

/**
 * @brief Event IDs
 * 
 */
typedef enum eEventId
{
    EVENT_TYPE__TEST_EVENT_1,
    EVENT_TYPE__TEST_EVENT_2,
    EVENT_TYPE__LED_MODE,
    SYSTEM_EVENT_COUNT
} eEventId;

/**
 * @brief Signal IDs
 * 
 */
typedef enum eSignalId
{
    SIGNAL_ID__TEST_SINGAL_1,
    SIGNAL_ID__TEST_SINGAL_2,
    SIGNAL_ID__TEST_SINGAL_3,
    SIGNAL_ID__LED_SEL_TIMEOUT,
    SIGNAL_ID__LED1_TIMEOUT,
    SIGNAL_ID__LED2_TIMEOUT,
    SIGNAL_ID__LED3_TIMEOUT,
    SIGNAL_ID__SELECTED_LED_CHANGED,
    SIGNAL_ID__LED_MODE_CHANGED,
    SIGNAL_ID__DEBOUNCE_TIMEOUT,
    SIGNAL_ID__LONG_PRESS_TIMEOUT
} eSignalId;

/**
 * @brief Signal Types
 * 
 */
typedef enum eSignalType
{
    SIGNAL_TYPE__ID,
    SIGNAL_TYPE__ID_U8,
    SIGNAL_TYPE__COUNT
} eSignalType;

/**
 * @brief Signal structure only signal ID
 * 
 */
typedef struct sSig_id
{
    eSignalId sig_id; // signal ID
} sSig_id;

/**
 * @brief Signal structure signal ID and unsigned 8 bit data 
 * 
 */
typedef struct sSig_id_u8
{
    eSignalId sig_id; // signal ID
    uint8_t data;     // data
} sSig_id_u8;

/**
 * @brief Signal structure general
 * 
 */
typedef struct sSig_gen
{
    eSignalType sig_type; // signal type
    void *ptr_sig;        // pointer of signal
} sSig_gen;

#endif //MICROS_TSE_CONFIG_H_
