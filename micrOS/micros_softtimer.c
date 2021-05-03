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
#include <string.h> //EKLENDI

#define TIMER_CALLBACK_TYPE_CALLBACK_FUNC 0
#define TIMER_CALLBACK_TYPE_TASK 1
#define TIMER_CALLBACK_TYPE_EVENT 2

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
    uint8_t key;         // timer key
    uint32_t start;      // starting time in millisecond
    uint32_t interval;   // interval time in millisecond
    bool type : 1;       // timer type one shot or periodic
    uint8_t cb_type : 2; // callback type;
    bool timeout : 1;    // timeout flag
    union
    {
        void (*pfn_timeout_cb)(void); // timeout callback function
        struct
        {
            union
            {
                eEventId event_id : 16;
                eTaskId task_id : 16;
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
 * @param ppNode Node which node's key = tm_key
 * @param ppPrev Previous node, if ppNode is first node this value set with NULL
 * @return true Timer found
 * @return false Timer not found
 */
static bool micros_timer_search(uint8_t tm_key, sMicros_tm_node **ppNode, sMicros_tm_node **ppPrev);

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

uint8_t micros_timer_event_publish(bool tm_type, bool cancelable, uint32_t interval, eEventId event_id, const sSig_gen *sig_gen)
{
    sMicros_timer timer;
    if (cancelable)
        timer.key = micros_timer_create_tmkey();
    else
        timer.key = 0;
    timer.cb_type = TIMER_CALLBACK_TYPE_EVENT;
    timer.event_id = event_id;
    timer.interval = interval;
    timer.sig_gen.ptr_sig = sig_gen->ptr_sig;
    timer.sig_gen.sig_type = sig_gen->sig_type;
    timer.start = time_counter;
    timer.timeout = 0;
    timer.type = tm_type;
    micros_timer_add(&timer);
    return timer.key;
}

uint8_t micros_timer_event_dispatch(bool tm_type, bool cancelable, uint32_t interval, eTaskId task_id, const sSig_gen *sig_gen)
{
    sMicros_timer timer;
    if (cancelable)
        timer.key = micros_timer_create_tmkey();
    else
        timer.key = 0;
    timer.cb_type = TIMER_CALLBACK_TYPE_TASK;
    timer.task_id = task_id;
    timer.interval = interval;
    timer.sig_gen.ptr_sig = sig_gen->ptr_sig;
    timer.sig_gen.sig_type = sig_gen->sig_type;
    timer.start = time_counter;
    timer.timeout = 0;
    timer.type = tm_type;
    micros_timer_add(&timer);
    return timer.key;
}

uint8_t micros_timer_start(bool tm_type, bool cancelable, uint32_t interval, void (*pfnCallbackFunc)(void))
{
    sMicros_timer timer;
    if (cancelable)
        timer.key = micros_timer_create_tmkey();
    else
        timer.key = 0;
    timer.cb_type = TIMER_CALLBACK_TYPE_CALLBACK_FUNC;
    timer.interval = interval;
    timer.pfn_timeout_cb = pfnCallbackFunc;
    timer.start = time_counter;
    timer.timeout = 0;
    timer.type = tm_type;
    micros_timer_add(&timer);
    return timer.key;
}

void micros_timer_cancel(uint8_t tm_key)
{
    sMicros_tm_node **ppNode, **ppPrev;
    if (micros_timer_search(tm_key, ppNode, ppPrev))
        micros_timer_delete(ppNode, ppPrev);
}

void micros_softtimer_init(void)
{
    //TODO: mcu timer initialize function will be added
}

void micros_softtimer_main(void)
{
    sMicros_tm_node *pNode = pTm_head;
    while (pNode != NULL)
    {
        if (pNode->tm.timeout == 0)
            continue;
        pNode->tm.timeout = 0;
        switch (pNode->tm.cb_type)
        {
        case TIMER_CALLBACK_TYPE_EVENT:
            micros_event_publish(pNode->tm.event_id, &(pNode->tm.sig_gen));
            break;

        case TIMER_CALLBACK_TYPE_TASK:
            micros_event_dispatch(&(pNode->tm.sig_gen), pNode->tm.task_id);
            break;

        case TIMER_CALLBACK_TYPE_CALLBACK_FUNC:
            pNode->tm.pfn_timeout_cb();
            break;
        }
        sMicros_tm_node *pTemp = pNode;
        pNode = pNode->next;
        switch (pTemp->tm.type)
        {
        case MICROS_TM_TYPE_ONE_SHOT:
            micros_timer_cancel(pTemp->tm.key);
            break;

        case MICROS_TM_TYPE_PERIODIC:
            pTemp->tm.start = time_counter;
            break;

        default:
            // TODO: error function will be added
            break;
        }
    }
}

static uint8_t micros_timer_create_tmkey(void)
{
    static uint8_t tm_key = 1;
    uint8_t key_start = tm_key;
    sMicros_tm_node **temp;
    while (micros_timer_search(tm_key, temp, temp))
    {
        tm_key++;
        if (tm_key == 0)
            tm_key = 1;
        if (tm_key == key_start)
        {
            // TODO: timer key full error will be added
            return 0;
        }
    }
    return tm_key;
}

static bool micros_timer_search(uint8_t tm_key, sMicros_tm_node **ppNode, sMicros_tm_node **ppPrev)
{
    sMicros_tm_node **ppNode_current = &pTm_head;
    ppPrev = NULL;
    while (*ppNode_current != NULL)
    {
        if ((*ppNode_current)->tm.key == tm_key)
        {
            *ppNode = *ppNode_current;
            return true;
        }
        ppPrev = ppNode_current;
        ppNode_current = &((*ppNode_current)->next);
    }
    *ppNode = *ppPrev = NULL;
    return false;
}

static void micros_timer_delete(sMicros_tm_node **ppNode, sMicros_tm_node **ppPrev)
{
    callstack_in_delete_timer = 1;
    if (*ppNode == pTm_head)
        pTm_head = (*ppNode)->next;
    else
        (*ppPrev)->next = (*ppNode)->next;
    micros_memory_deallocate((*ppNode)->tm.sig_gen.ptr_sig, sig_size[(*ppNode)->tm.sig_gen.sig_type]);
    micros_memory_deallocate(*ppNode, sizeof(sMicros_tm_node));
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
        return;
    }
    memcpy(&(newNode->tm), timer, sizeof(sMicros_timer));
    newNode->tm.sig_gen.ptr_sig = micros_memory_allocate(sig_size[newNode->tm.sig_gen.sig_type]);
    memcpy(newNode->tm.sig_gen.ptr_sig, timer->sig_gen.ptr_sig, sig_size[newNode->tm.sig_gen.sig_type]);
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
        if (pNode->tm.interval >= micros_timer_calc_elapsed_time(pNode->tm.start, time_counter))
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
