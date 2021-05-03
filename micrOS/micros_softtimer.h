/**
 * @file micros_softtimer.h
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

#ifndef MICROS_SOFTTIMER_H_
#define MICROS_SOFTTIMER_H_

/**
 * @brief micros softtimer initialize function
 * 
 */
void micros_softtimer_init(void);

/**
 * @brief Softtimer main function. This function control timeout flags and
 * if necessary send signals to tasks
 */
void micros_softtimer_main(void);

#endif // MICROS_SOFTTIMER_H_
