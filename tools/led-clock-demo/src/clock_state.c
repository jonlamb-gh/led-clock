/**
 * @file clock_state.c
 * @brief TODO.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "gui_util.h"
#include "clock_state.h"

static void led_off(
        clock_digit_led_s * const led)
{
    led->rgb[0] = CLOCK_PWM_DISABLED;
    led->rgb[1] = CLOCK_PWM_DISABLED;
    led->rgb[2] = CLOCK_PWM_DISABLED;
}

static void blend_digits(
        const uint16_t ticks,
        clock_digit_led_s * const cur_led,
        clock_digit_led_s * const next_led)
{
    // ticks ranges from 0:CLOCK_TICK_PER_LED
    const uint16_t max = CLOCK_PWM_RESOLUTION;
    const uint16_t duty_per_tick = (max / CLOCK_TICK_PER_LED);
    const uint16_t delta = ticks * duty_per_tick;

    // TODO - sanity checks
    cur_led->rgb[0] = CLOCK_PWM_RESOLUTION - delta;

    next_led->rgb[0] = delta;

    /*
    printf(
            "%d %d %d %d\n",
            (int) ticks,
            (int) max,
            (int) duty_per_tick,
            (int) delta);
    */
}

void clock_state_init(
        clock_state_s * const state)
{
    state->ticks = 0;
    state->enabled = 0;
    state->next_index = 1;

    uint8_t idx;
    for(idx = 0; idx < CLOCK_DIGIT_LED_COUNT; idx += 1)
    {
        led_off(&state->digits[idx]);
    }

    // TODO - need a set-time or initial time to set initial state
    state->digits[0].rgb[0] = CLOCK_PWM_RESOLUTION;
}

void clock_state_tick(
        clock_state_s * const state)
{
    // first set for seconds, then blend minute/hour

    const uint16_t seconds = (state->ticks / CLOCK_TICK_PER_SEC);
    const uint16_t index = (seconds / CLOCK_SEC_PER_LED);
    assert(index < CLOCK_DIGIT_LED_COUNT);

    if(index == state->next_index)
    {
        // transition digits

        // this is just for testing, bad index scheme
        const uint16_t prev_index = (index - 1) % CLOCK_DIGIT_LED_COUNT;
        
        printf("%d %d\n", (int) index, (int) prev_index);

        if(index == 0)
        {
            state->digits[CLOCK_DIGIT_LED_COUNT - 1].rgb[0] = CLOCK_PWM_DISABLED;
        }
        else
        {
            state->digits[prev_index].rgb[0] = CLOCK_PWM_DISABLED;
        }

        state->next_index = (index + 1) % CLOCK_DIGIT_LED_COUNT;
    }

    blend_digits(
            state->ticks % CLOCK_TICK_PER_LED,
            &state->digits[index],
            &state->digits[state->next_index]);

    /*
    if(index != state->prev_index)
    {
        // transition digits
        state->digits[state->prev_index].rgb[0] = 0;

        state->prev_index = index;
    }
    else
    {
        // TODO - start blending immediately rather than this
        state->digits[index].rgb[0] = CLOCK_PWM_RESOLUTION;
    }
    */

    state->ticks += 1;

    if(state->ticks >= CLOCK_TICKS_PER_REVOLUTION)
    {
        state->ticks = 0;
    }
}
