/**
 * @file micros_core.c
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

#include "micros_core.h"
#include "micros_task_config.h"
#include "micros.h"
#include "micros_config.h"
#include "micros_mem.h"
#include "micros_softtimer.h"
#include <string.h>

/**
 * @brief Event subscribe list structure
 * 
 */
typedef struct sEventSubs
{
    eTaskId task_id;
    struct sEventSubs *next;
} sEventSubs;

/**
 * @brief Add signal to end of signal linked list
 * 
 * @param ppHead Head signal node pointer pointer
 * @param ppTail Tail signal node pointer pointer
 * @param signal Adding signal pointer
 */
static void micros_enqueue_signal(sSig_node **ppHead, sSig_node **ppTail, const sSig_gen *signal);

/**
 * @brief Delete signal to begin of signal linked list
 * 
 * @param ppHead Head signal node pointer pointer
 * @param ppTail Tail signal node pointer pointer
 * @return sSig_gen Signal which begin of signal linked list
 */
static sSig_gen micros_dequeue_signal(sSig_node **ppHead, sSig_node **ppTail);

/**
 * @brief Return signal linked list empty or not
 * 
 * @param pHead Head signal node pointer
 * @return true If linked list empty
 * @return false If linked list not empty
 */
static bool micros_is_empty_sig_list(sSig_node *pHead);

/**
 * @brief Add task to event subscribe list
 * 
 * @param ppNode Pointer pointer of head event list
 * @param task_id Task ID
 */
static void micros_add_task_subs(sEventSubs **ppNode, eTaskId task_id);

/**
 * @brief Delete task to event subscribe list
 * 
 * @param ppNode Pointer pointer of head event list
 * @param task_id Task ID
 */
static void micros_remove_task_subs(sEventSubs **ppNode, eTaskId task_id);

sEventSubs *pEvent_subs[SYSTEM_EVENT_COUNT];

void micros_allTaskInit(void)
{
    for (uint16_t task_id = 0; task_id < SYSTEM_TASK_COUNT; task_id++)
        micros_task[task_id].pfnInitTask();
}

void micros_taskInit(eTaskId task_id)
{
    micros_task[task_id].pfnInitTask();
}

void micros_main(void)
{
    micros_softtimer_main();
    for (uint16_t task_id = 0; task_id < SYSTEM_TASK_COUNT; task_id++)
    {
        if (micros_task[task_id].run_state == false)
            continue;
        while (micros_is_empty_sig_list(micros_task[task_id].pSig_head) == false)
        {
            sSig_gen sig_gen = micros_dequeue_signal(&(micros_task[task_id].pSig_head), &(micros_task[task_id].pSig_tail));
            micros_task[task_id].pfnGetSignal(&sig_gen);
            micros_memory_deallocate(sig_gen.ptr_sig, sig_size[sig_gen.sig_type]);
        }
        micros_task[task_id].pfnMainTask();
    }
}

void micros_init(void)
{
    for (uint16_t task_id = 0; task_id < SYSTEM_TASK_COUNT; task_id++)
    {
        micros_task[task_id].pSig_head = NULL;
        micros_task[task_id].pSig_tail = NULL;
    }
    micros_softtimer_init();
}

void micros_task_runstate_set(eTaskId task_id, bool run_state)
{
    if (task_id < SYSTEM_TASK_COUNT)
        micros_task[task_id].run_state = run_state;
}

bool micros_task_runstate_get(eTaskId task_id)
{
    if (task_id < SYSTEM_TASK_COUNT)
        return micros_task[task_id].run_state;
    return false;
}

void micros_event_dispatch(const sSig_gen *sig_gen, eTaskId task_id)
{
    micros_enqueue_signal(&(micros_task[task_id].pSig_head), &(micros_task[task_id].pSig_tail), sig_gen);
}

void micros_event_publish(eEventId event_id, const sSig_gen *sig_gen)
{
    if (event_id >= SYSTEM_EVENT_COUNT) // unknown event id
        return;
    sEventSubs *event_sub = pEvent_subs[event_id];
    while (event_sub != NULL)
    {
        micros_event_dispatch(sig_gen, event_sub->task_id);
        event_sub = (sEventSubs *)event_sub->next;
    }
}

void micros_event_substate_set(eEventId event_id, eTaskId task_id, bool sub_state)
{
    if (sub_state)
        micros_add_task_subs(&pEvent_subs[event_id], task_id);
    else
        micros_remove_task_subs(&pEvent_subs[event_id], task_id);
}

static void micros_enqueue_signal(sSig_node **ppHead, sSig_node **ppTail, const sSig_gen *signal)
{
    sSig_node *pNew = micros_memory_allocate(sizeof(sSig_node));
    if (pNew == NULL)
    {
        // TODO: Add error function
        return;
    }
    pNew->sig_gen.ptr_sig = micros_memory_allocate(sig_size[signal->sig_type]);
    if (pNew->sig_gen.ptr_sig == NULL)
    {
        // TODO: Add free signal general allocated memory function
        // TODO: Add error function
        return;
    }
    // memory copy for specific signal types
    memcpy(pNew->sig_gen.ptr_sig, signal->ptr_sig, sig_size[signal->sig_type]);
    pNew->sig_gen.sig_type = signal->sig_type;
    pNew->next = NULL;

    // if empty, insert node at head
    if (micros_is_empty_sig_list(*ppHead))
        *ppHead = pNew;
    else
        (*ppTail)->next = pNew;
    *ppTail = pNew;
}

static sSig_gen micros_dequeue_signal(sSig_node **ppHead, sSig_node **ppTail)
{
    sSig_gen retval ={.sig_type = (*ppHead)->sig_gen.sig_type, .ptr_sig = (*ppHead)->sig_gen.ptr_sig};
    sSig_node *pTemp = *ppHead;
    *ppHead = (*ppHead)->next;
    // if queue is empty
    if(*ppHead == NULL)
        *ppTail = NULL;
    micros_memory_deallocate(pTemp,sizeof(sSig_node));
    return retval;
}

static bool micros_is_empty_sig_list(sSig_node *pHead)
{
    return pHead == NULL;
}

static void micros_add_task_subs(sEventSubs **ppNode, eTaskId task_id)
{
    while (*ppNode != NULL)
    {
        if ((*ppNode)->task_id == task_id) // already added list
            return;
        ppNode = (sEventSubs **)&(*ppNode)->next;
    }
    *ppNode = micros_memory_allocate(sizeof(sEventSubs));
    // control memory allocation
    if (*ppNode == NULL)
    {
        // TODO: error handler function will be added
        return;
    }
    (*ppNode)->task_id = task_id;
    (*ppNode)->next = NULL;
}

static void micros_remove_task_subs(sEventSubs **ppNode, eTaskId task_id)
{
    sEventSubs **ppPrevNode;
    if ((*ppNode) == NULL) // event list is empty
        return;
    while ((*ppNode)->task_id != task_id)
    {
        (*ppNode) = (sEventSubs *)(*ppNode)->next;
        if ((*ppNode) == NULL) // not detect this task id in list
            return;
        ppPrevNode = ppNode;
    }
    (*ppPrevNode)->next = (*ppNode)->next;
    micros_memory_deallocate(*ppNode, sizeof(sEventSubs));
}
