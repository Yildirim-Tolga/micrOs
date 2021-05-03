/**
 * @file micros_mem.h
 * @author Tolga Yildirim (tolga.yildirimmm@gmail.com)
 * @brief Control micros memory allocate and deallocate functions
 * @version 1.0
 * @date 2021-04-30
 * 
 * @copyright
 * 
 * Version History:  +,-,/,*: add, remove, change, fix
 *  v1.0:   First release.
 */

#ifndef MICROS_MEM_H_
#define MICROS_MEM_H_

#include <stdlib.h>

/**
 * @brief Memory allocate function
 * 
 * @param size Memory size
 * @return void* Memory address pointer
 */
void *micros_memory_allocate(size_t size);

/**
 * @brief Memory deallocate function
 * 
 * @param adr Memory address pointer
 * @param size Memory size
 */
void micros_memory_deallocate(void *adr, size_t size);

#endif // MICROS_MEM_H_
