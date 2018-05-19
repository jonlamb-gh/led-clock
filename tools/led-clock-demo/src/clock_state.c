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
    led->rgb[0] = 0;
    led->rgb[1] = 0;
    led->rgb[2] = 0;
}

void clock_state_init(
        clock_state_s * const state)
{
    state->ticks = 0;
    state->enabled = 0;
    state->prev_index = 0;

    uint8_t idx;
    for(idx = 0; idx < CLOCK_DIGIT_LED_COUNT; idx += 1)
    {
        led_off(&state->digits[idx]);
    }
}

void clock_state_tick(
        clock_state_s * const state)
{
    // first set for seconds, then blend minute/hour

    const uint16_t seconds = (state->ticks / CLOCK_TICK_PER_SEC);
    const uint16_t index = (seconds / CLOCK_SEC_PER_LED);
    assert(index < CLOCK_DIGIT_LED_COUNT);

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

    state->ticks += 1;

    if(state->ticks >= CLOCK_TICKS_PER_REVOLUTION)
    {
        state->ticks = 0;
    }
}
