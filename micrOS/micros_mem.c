/**
 * @file micros_mem.c
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

#include "micros_config.h"
#include "micros_mem.h"

#if defined(MICROS_USE_DYNAMIC_MEMORY_PRE_ALLOCATED)
// TODO: preallocated dynamic memory malloc will be added
void *micros_memory_allocate(size_t size)
{
}

void micros_memory_deallocate(void *adr, size_t size)
{
}
#elif defined(MICROS_USE_DYNAMIC_MEMORY)
void *micros_memory_allocate(size_t size)
{
    return malloc(size);
}

void micros_memory_deallocate(void *adr, size_t size)
{
    if (size > 0)
        free(adr);
}
#elif defined(MICROS_USE_STATIC_MEMORY)
// TODO: static memory allocation will be added
void *micros_memory_allocate(size_t size)
{
}

void micros_memory_deallocate(void *adr, size_t size)
{
}
#else
#error "Please define memory allocation type in 'micros_config.h' file"
#endif
