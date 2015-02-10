/*! \file timer.h
 *
 * \copyright (C) Copyright 2013 University of Antwerp (http://www.cosys-lab.be) and others.\n
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.\n
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * \author maarten.weyn@uantwerpen.be
 * \daniel.vandenakker@uantwerpen.be
 *
 */

#ifndef HW_TIMER_H_
#define HW_TIMER_H_

#include <stdbool.h>
#include <stdint.h>

#include "errors.h"
#include "platform.h"

#ifndef PLATFORM_NUM_TIMERS
    #error The platform should define the number of available timers
#endif

enum
{
    HWTIMER_FREQ_1MS = 0,
    HWTIMER_TICKS_1MS = 1024,
    HWTIMER_FREQ_32K = 1,
    HWTIMER_TICKS_32K = 32768,
    HWTIMER_NUM = PLATFORM_NUM_TIMERS,
};

/*! \brief Timer callback definition
 * 
 */
typedef void (*timer_callback_t)();

/*! \brief Type definition of the type used to identify timers;
 * 
 */
typedef uint8_t hwtimer_id_t;

/*! \brief Type definition of the timer clock ticks
 * 
 */
typedef uint16_t hwtimer_tick_t;

/*! \brief Initialise a hardware timer.
 *
 * \param timer_id		The id of the timer to initialise. timer_id must be between 0 and HWTIMER_NUM
 * \param frequency		The frequency at which the timer shoudl run. One of the HWTIMER_FREQ_ values
 * \param compare_callback	The function to call when the timer reaches the value configured with 
 *				hw_timer_set_value. If this parameter is 0x0, no callback will occur.
 * \param overflow_callback	The function to call when the timer overflows back to zero. This function is 
 *				called regardless of whether the timer is scheduled or not. overflow_callback
 *				is *NOT* called if the timer counter is reset manually by calling 
 *				hw_timer_counter_reset. If this value is 0x0, no callback will occur when the
 *				timer overflows to zero.
 *
 * \return error_t		SUCCESS if the timer was initialised successfully, 
 *				EALREADY if the timer was already configured,
 *				ESIZE if an invalid timer_id was specified (out of range)
 *				EINVAL if the requested frequency is not supported by the timer
 */
error_t hw_timer_init(hwtimer_id_t timer_id, uint8_t frequency, timer_callback_t compare_callback, timer_callback_t overflow_callback);

/*! \brief Get the current timer value
 * 
 *  \param timer_id	the id of the timer from which the value is to be retrieved. must be between 0 and HWTIMER_NUM
 *
 *  \return		the current value of the timer if the specified timer_id is valid
 *			and the corresponding timer has been initialised. 0 otherwise
 */
hwtimer_tick_t hw_timer_getvalue(hwtimer_id_t timer_id);

/*! \brief Schedule a hardware timer to fire at a specific clock tick
 *
 * This function not only sets the correct timer 'comparator' value,
 * but also disables, clears and resets the correct interrupt flag to ensure
 * that the timer will fire at the given clock tick. When the timer fires, the 
 * timer interrupts are also disabled to ensure that the timer only fires once.
 *
 * This function however does *NOT* reset the timer counter value.
 * ( hw_counter_reset() can be used for this purpose). If the specified tick
 * value is lower than the current value of the timer, the timer will not fire until 
 * the counter has looped around and reached the specified value
 *
 * If the timer was already scheduled it is cancelled and subsequently rescheduled.
 * Please note that the timer callback is done from an interrupt context (all interrupts disabled)
 * and that the processing of the timer should be as minimal as possible
 * 
 *
 * \param	timer_id	the id of the timer to schedule
 * \param	tick		the exact clock tick at which the timer should fire
 *
 * \return	error_t		SUCCESS if the timer was scheduled successfully
 *		 	 	 	 	ESIZE if an invalid timer id was specified
 * 						EOFF if the timer was not yet configured
 *
 */
error_t hw_timer_schedule(hwtimer_id_t timer_id, hwtimer_tick_t tick );

/*! \brief Schedule a hardware timer to fire with a specified delay
 *
 * This is a shorthand for calling
 * \code{.c}
 * 	hw_timer_schedule_at(timer_id, hw_timer_getvalue(timer_id) + delay);
 * \endcode
 * \param	timer_id	the id of the timer to schedule
 * \param	delay		the delay before the timer fires
 *
 * \return	error_t		SUCCESS if the timer was scheduled successfully
 *		 	 	 	 	ESIZE if an invalid timer id was specified
 * 						EOFF if the timer was not yet configured
 *
 */
static inline error_t hw_timer_schedule_delay(hwtimer_id_t timer_id, hwtimer_tick_t delay)
{
    return hw_timer_schedule(timer_id, hw_timer_getvalue(timer_id) + delay);
}

/*! \brief Cancel a running timer
 *
 * \param	timer_id	the id of the timer to cancel
 *
 * \return	error_t		SUCCESS if the timer was scheduled successfully
 *		 	 	 	 	ESIZE if an invalid timer id was specified
 * 						EOFF if the timer was not yet configured
 *
 */
error_t hw_timer_cancel(hwtimer_id_t timer_id);

/*! \brief Reset the counter value of the timer
 *
 * Resetting the timer counter also cancels the timer if it was running. 
 * (This to prevent any timings from being messed up by a changed timer value).
 * It should also be noted that resetting the timer value does *NOT* trigger
 * the overflow_callback function to be called.
 *
 * \param	timer_id	the id of the timer to cancel
 *
 * \return	error_t		SUCCESS is the timer was cancelled succesfully
 *						ESIZE if an invalid timer id was specified
 * 						EOFF if the timer was not yet configured
 */
error_t hw_timer_counter_reset(hwtimer_id_t timer_id);

/*! \brief Check whether a timer overflow interrupt is pending
 *
 * When an overflow interupt is pending, the overflow itself
 * has already happened but the corresponding ISR has not been 
 * triggered yet.
 * 
 * \return bool	true if an overflow is pending, false otherwise
 */
bool hw_timer_is_overflow_pending(hwtimer_id_t id);

/*! \brief Check whether a timer interrupt is pending
 *
 * When a timer interrupt is pending, the timer counter has already 
 * reached the value configured with hw_timer_schedule() but the
 * corresponding ISR has not yet been triggered.
 *
 * \return bool	true if a timer interrupt is pending, false otherwise
 */
bool hw_timer_is_interrupt_pending(hwtimer_id_t id);

#endif /* HW_TIMER_H_ */
