/**
 * @file micros_softtimer.c
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

#include "micros.h"
#include "micros_mem.h"
#include "micros_mcu_timer.h"
#include "micros_config.h"
#include <string.h>

#define TIMER_CALLBACK_TYPE_CB_FUNC 0
#define TIMER_CALLBACK_TYPE_TASK 1
#define TIMER_CALLBACK_TYPE_EVENT 2
#define TIMER_CALLBACK_TYPE_CB_FUNC_PARAM 3

/* ========================================  Start of section using anonymous unions  ======================================== */
#if defined(__CC_ARM)
#pragma push
#pragma anon_unions
#elif defined(__ICCARM__)
#pragma language = extended
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc11-extensions"
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#pragma clang diagnostic ignored "-Wgnu-anonymous-struct"
#pragma clang diagnostic ignored "-Wnested-anon-types"
#elif defined(__GNUC__)
/* anonymous unions are enabled by default */
#elif defined(__TMS470__)
/* anonymous unions are enabled by default */
#elif defined(__TASKING__)
#pragma warning 586
#elif defined(__CSMC__)
/* anonymous unions are enabled by default */
#else
#warning Not supported compiler type
#endif
/**
 * @brief Timer structure
 * 
 */
typedef struct sMicros_timer
{
    const uint8_t key;         // timer key
    uint32_t start;      // starting time in millisecond
    uint32_t interval;   // interval time in millisecond
    const uint8_t type : 1;    // timer type one shot or periodic
    const uint8_t cb_type : 2; // callback type;
    uint8_t timeout : 1; // timeout flag
    union
    {
        void (*pfn_timeout_cb)(void); // timeout callback function

        struct //timeout callback function with param
        {
            void (*pfn_timeout_cb_param)(const void *);
            const void *param;
        };

        struct
        {
            union
            {
                const eEventId event_id : 16;
                const eTaskId task_id : 16;
            };
            sSig_gen sig_gen;
        };
    };
} sMicros_timer;
/* =========================================  End of section using anonymous unions  ========================================= */
#if defined(__CC_ARM)
#pragma pop
#elif defined(__ICCARM__)
/* leave anonymous unions enabled */
#elif (__ARMCC_VERSION >= 6010050)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
/* anonymous unions are enabled by default */
#elif defined(__TMS470__)
/* anonymous unions are enabled by default */
#elif defined(__TASKING__)
#pragma warning restore
#elif defined(__CSMC__)
/* anonymous unions are enabled by default */
#endif

/**
 * @brief Timer linked list structure
 * 
 */
typedef struct sMicros_tm_node
{
    sMicros_timer tm;
    struct sMicros_tm_node *next;
} sMicros_tm_node;

/**
 * @brief Create timer key for new timer
 * 
 * @return uint8_t Timer key
 */
static uint8_t micros_timer_create_tmkey(void);

/**
 * @brief Search timer with timer key function. Return find or not. If find set ppNode and ppPrev
 * 
 * @param tm_key Timer key
 * @param pppNode Node which node's key = tm_key
 * @param pppPrev Previous node, if ppNode is first node this value set with NULL
 * @return true Timer found
 * @return false Timer not found
 */
static uint8_t micros_timer_search(uint8_t tm_key, sMicros_tm_node ***pppNode, sMicros_tm_node ***ppPrev);

/**
 * @brief Delete timer node with node address and prev node address
 * 
 * @param ppNode Node for delete
 * @param ppPrev Previous node
 */
static void micros_timer_delete(sMicros_tm_node **ppNode, sMicros_tm_node **ppPrev);

/**
 * @brief Create timer
 * 
 * @param timer timer variables
 */
static void micros_timer_add(sMicros_timer *timer);

/**
 * @brief Systic timer callback function
 * 
 */
void micros_timer_mcu_cb(void);

/**
 * @brief Control timeout if necessary set timeout flags
 * 
 */
static void micros_timer_timeout_ctrl(void);

/**
 * @brief Calculate elapsed time
 * 
 * @param start Start time
 * @param current Current time
 * @return uint32_t Elapsed time
 */
static uint32_t micros_timer_calc_elapsed_time(uint32_t start, uint32_t current);

sMicros_tm_node *pTm_head = NULL;

static volatile uint8_t callstack_in_create_timer = 0;
static volatile uint8_t callstack_in_delete_timer = 0;
static volatile uint8_t timeout_ctrl_break = 0;
static volatile uint32_t time_counter = 0;

extern const size_t sig_size[SIGNAL_TYPE__COUNT];

uint8_t micros_timer_event_publish(uint8_t tm_type, uint8_t cancelable, uint32_t interval, eEventId event_id, const sSig_gen *sig_gen)
{
    uint8_t key = 0;
    if (cancelable)
        key = micros_timer_create_tmkey();

    sMicros_timer timer = {
        .key = key,
        .cb_type = TIMER_CALLBACK_TYPE_EVENT,
        .event_id = event_id,
        .interval = interval,
        .sig_gen.ptr_sig = sig_gen->ptr_sig,
        .sig_gen.sig_type = sig_gen->sig_type,
        .start = time_counter,
        .timeout = 0,
        .type = tm_type
    };
    
    micros_timer_add(&timer);
    return timer.key;
}

uint8_t micros_timer_event_dispatch(uint8_t tm_type, uint8_t cancelable, uint32_t interval, eTaskId task_id, const sSig_gen *sig_gen)
{
    uint8_t key = 0;
    if (cancelable)
        key = micros_timer_create_tmkey();

    sMicros_timer timer = {
        .key = key,
        .cb_type = TIMER_CALLBACK_TYPE_TASK,
        .task_id = task_id,
        .interval = interval,
        .sig_gen.ptr_sig = sig_gen->ptr_sig,
        .sig_gen.sig_type = sig_gen->sig_type,
        .start = time_counter,
        .timeout = 0,
        .type = tm_type
    };
    
    micros_timer_add(&timer);
    return timer.key;
}

uint8_t micros_timer_start(uint8_t tm_type, uint8_t cancelable, uint32_t interval, void (*pfnCallbackFunc)(void))
{
    uint8_t key = 0;
    if (cancelable)
        key = micros_timer_create_tmkey();

    sMicros_timer timer = {
        .key = key,
        .cb_type = TIMER_CALLBACK_TYPE_CB_FUNC,
        .interval = interval,
        .pfn_timeout_cb = pfnCallbackFunc,
        .start = time_counter,
        .timeout = 0,
        .type = tm_type
    };
    
    micros_timer_add(&timer);
    return timer.key;
}

uint8_t micros_timer_start_param(uint8_t tm_type, uint8_t cancelable, uint32_t interval, void (*pfnCallbackFunc)(const void *),const void *param)
{
    uint8_t key = 0;
    if (cancelable)
        key = micros_timer_create_tmkey();

    sMicros_timer timer = {
        .key = key,
        .cb_type = TIMER_CALLBACK_TYPE_CB_FUNC_PARAM,
        .interval = interval,
        .pfn_timeout_cb_param = pfnCallbackFunc,
        .param = param,
        .start = time_counter,
        .timeout = 0,
        .type = tm_type
    };

    micros_timer_add(&timer);
    return timer.key;
}

void micros_timer_cancel(uint8_t tm_key)
{
    sMicros_tm_node **ppNode, **ppPrev;
    if (micros_timer_search(tm_key, &ppNode, &ppPrev))
        micros_timer_delete(ppNode, ppPrev);
}

void micros_softtimer_init(void)
{
    micros_mcu_timer_init(micros_timer_mcu_cb);
}

void micros_softtimer_main(void)
{
    sMicros_tm_node **ppNode = &pTm_head;
    while ((*ppNode) != NULL)
    {
        if ((*ppNode)->tm.timeout == 0)
        {
            ppNode = &((*ppNode)->next);
            continue;
        }
        (*ppNode)->tm.timeout = 0;
        switch ((*ppNode)->tm.cb_type)
        {
        case TIMER_CALLBACK_TYPE_EVENT:
            micros_event_publish((*ppNode)->tm.event_id, &((*ppNode)->tm.sig_gen));
            break;

        case TIMER_CALLBACK_TYPE_TASK:
            micros_event_dispatch(&((*ppNode)->tm.sig_gen), (*ppNode)->tm.task_id);
            break;

        case TIMER_CALLBACK_TYPE_CB_FUNC:
            (*ppNode)->tm.pfn_timeout_cb();
            break;
        case TIMER_CALLBACK_TYPE_CB_FUNC_PARAM:
            (*ppNode)->tm.pfn_timeout_cb_param((*ppNode)->tm.param);
            break;
        default:
            // TODO: error function will be added
            while (1)
                ;
            break;
        }
        sMicros_tm_node **ppTemp = ppNode;
        ppNode = &((*ppNode)->next);
        switch ((*ppTemp)->tm.type)
        {
        case MICROS_TM_TYPE_ONE_SHOT:
            micros_timer_cancel((*ppTemp)->tm.key);
            break;

        case MICROS_TM_TYPE_PERIODIC:
            (*ppTemp)->tm.start = time_counter;
            break;

        default:
            // TODO: error function will be added
            while (1)
                ;
            break;
        }
    }
}

static uint8_t micros_timer_create_tmkey(void)
{
    static uint8_t tm_key = 1;
    uint8_t key_start = tm_key;
    sMicros_tm_node **temp1, **temp2;
    do
    {
        tm_key++;
        if (tm_key == 0)
            tm_key = 1;
        if (!micros_timer_search(tm_key, &temp1, &temp2))
        {
            return tm_key;
        }
    } while (tm_key == key_start);

    // TODO: timer key full error will be added
    while (1)
        ;
    return 0;
}

static uint8_t micros_timer_search(uint8_t tm_key, sMicros_tm_node ***pppNode, sMicros_tm_node ***pppPrev)
{
    sMicros_tm_node **ppNode_current = &pTm_head;
    *pppPrev = NULL;
    while (*ppNode_current != NULL)
    {
        if ((*ppNode_current)->tm.key == tm_key)
        {
            *pppNode = ppNode_current;
            return TRUE;
        }
        *pppPrev = ppNode_current;
        ppNode_current = &((*ppNode_current)->next);
    }
    *pppNode = *pppPrev = NULL;
    return FALSE;
}

static void micros_timer_delete(sMicros_tm_node **ppNode, sMicros_tm_node **ppPrev)
{
    callstack_in_delete_timer = 1;
    sMicros_tm_node **ppHead = &pTm_head;
    if (*ppNode == pTm_head)
        ppHead = &((*ppNode)->next);
    else
        (*ppPrev)->next = (*ppNode)->next;
    if ((*ppNode)->tm.cb_type == TIMER_CALLBACK_TYPE_TASK || (*ppNode)->tm.cb_type == TIMER_CALLBACK_TYPE_EVENT)
        micros_memory_deallocate((*ppNode)->tm.sig_gen.ptr_sig, sig_size[(*ppNode)->tm.sig_gen.sig_type]);
    micros_memory_deallocate(*ppNode, sizeof(sMicros_tm_node));
    pTm_head = *ppHead;
    callstack_in_delete_timer = 0;
    if (timeout_ctrl_break)
        micros_timer_timeout_ctrl(); // timeout control available now
}

static void micros_timer_add(sMicros_timer *timer)
{
    callstack_in_create_timer = 1;
    sMicros_tm_node *newNode = micros_memory_allocate(sizeof(sMicros_tm_node));
    ;
    if (newNode == NULL)
    {
        // TODO: Error function will be added
        while (1)
            ;
        return;
    }
    memcpy(&(newNode->tm), timer, sizeof(sMicros_timer));
    if (newNode->tm.cb_type == TIMER_CALLBACK_TYPE_TASK || newNode->tm.cb_type == TIMER_CALLBACK_TYPE_EVENT)
    {
        newNode->tm.sig_gen.ptr_sig = micros_memory_allocate(sig_size[newNode->tm.sig_gen.sig_type]);
        memcpy(newNode->tm.sig_gen.ptr_sig, timer->sig_gen.ptr_sig, sig_size[newNode->tm.sig_gen.sig_type]);
    }
    newNode->next = pTm_head;
    pTm_head = newNode;
    callstack_in_create_timer = 0;
    if (timeout_ctrl_break)
        micros_timer_timeout_ctrl(); // timeout control available now
}

void micros_timer_mcu_cb(void)
{
    time_counter += MICROS_MCU_TIMER_PERIOD;
    if (callstack_in_create_timer || callstack_in_delete_timer)
    {
        // timeout control not available now
        timeout_ctrl_break = 1;
        return;
    }
    micros_timer_timeout_ctrl();
}

static void micros_timer_timeout_ctrl(void)
{
    timeout_ctrl_break = 0;
    sMicros_tm_node *pNode = pTm_head;
    while (pNode != NULL)
    {
        if (pNode->tm.interval < micros_timer_calc_elapsed_time(pNode->tm.start, time_counter))
            pNode->tm.timeout = 1;
        pNode = pNode->next;
    }
}

static uint32_t micros_timer_calc_elapsed_time(uint32_t start, uint32_t current)
{
    if (start <= current)
        return current - start;
    return (0xFFFFFFFF - start) + current;
}
