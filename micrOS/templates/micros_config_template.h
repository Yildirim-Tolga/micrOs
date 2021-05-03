/**
 * @file micros_config.h
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

#ifndef MICROS_CONFIG_H_
#define MICROS_CONFIG_H_

// comment or uncomment
#define MICROS_USE_DYNAMIC_MEMORY
#ifdef MICROS_USE_DYNAMIC_MEMORY
// comment or uncomment
// #define MICROS_USE_DYNAMIC_MEMORY_PRE_ALLOCATED
#else
#define MICROS_USE_STATIC_MEMORY
#endif

#if defined(MICROS_USE_DYNAMIC_MEMORY_PRE_ALLOCATED) || defined(MICROS_USE_STATIC_MEMORY)
#define MICROS_MAX_MEM_SIZE 512
#endif

#define NULL 0

#define TASK_STATE_RUN true
#define TASK_STATE_STOP false

#endif //MICROS_TASK_CONFIG_