/**
 * @file clock_state.c
 * @brief TODO.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "gui_util.h"
#include "clock_state.h"

/*
typedef struct
{
    uint16_t prev;
    uint16_t cur;
    uint16_t next;
    uint8_t color;
} digit_indices_s;
*/

static void init_indices(
        const uint8_t color_channel,
        clock_digit_indices_s * const indices)
{
    indices->color_channel = color_channel;
    indices->prev = CLOCK_DIGIT_LED_COUNT - 1;
    indices->cur = 0;
    indices->next = 1;
}

static uint8_t update_seconds_indices(
        const clock_time_s * const time,
        clock_digit_indices_s * const indices)
{
    uint8_t did_transition = 0;

    indices->cur = (time->seconds / CLOCK_SEC_PER_LED);
    assert(indices->cur < CLOCK_DIGIT_LED_COUNT);

    if(indices->cur == indices->next)
    {
        did_transition = 1;
    }

    indices->next = (indices->cur + 1) % CLOCK_DIGIT_LED_COUNT;

    if(indices->cur == 0)
    {
        indices->prev = CLOCK_DIGIT_LED_COUNT - 1;
    }
    else
    {
        indices->prev = (indices->cur - 1) % CLOCK_DIGIT_LED_COUNT;
    }

    return did_transition;
}

static void update_indices(
        const clock_digit_index_kind kind,
        const clock_time_s * const time,
        clock_state_s * const state,
        clock_digit_indices_s * const indices)
{
    uint8_t did_transition = 0;

    if(kind == CLOCK_DIGIT_INDEX_SECONDS)
    {
        did_transition = update_seconds_indices(time, indices);
    }
    else
    {
        assert(kind != kind);
    }

    if(did_transition != 0)
    {
        state->digits[indices->prev].rgb[indices->color_channel] = CLOCK_PWM_DISABLED;
    }
}

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
    (void) memset(state, 0, sizeof(*state));

    state->enabled = 0;

    uint8_t idx;
    for(idx = 0; idx < CLOCK_DIGIT_LED_COUNT; idx += 1)
    {
        led_off(&state->digits[idx]);
    }

    init_indices(
            0,
            &state->indices[CLOCK_DIGIT_INDEX_SECONDS]);
    init_indices(
            1,
            &state->indices[CLOCK_DIGIT_INDEX_MINUTES]);
    init_indices(
            2,
            &state->indices[CLOCK_DIGIT_INDEX_HOURS]);

    // TODO - need a set-time or initial time to set initial state
    state->digits[0].rgb[0] = CLOCK_PWM_RESOLUTION;

    state->time.ticks = 0;
    state->time.seconds = 0;
    state->time.minutes = 15;
    state->time.hours = 5;
}

// replace with a set-time function
void clock_state_tick(
        clock_state_s * const state)
{
    state->time.ticks += 1;

    if(state->time.ticks >= CLOCK_TICKS_PER_REVOLUTION)
    {
        state->time.ticks = 0;
    }

    state->time.seconds = (state->time.ticks / CLOCK_TICK_PER_SEC);

    // TESTING - time is normally provided, static hour/minute
    //state->time.minues = (state->ticks / CLOCK_TICK_PER_SEC);
    //state->time.hours = (state->ticks / CLOCK_TICK_PER_SEC);

    update_indices(
            CLOCK_DIGIT_INDEX_SECONDS,
            &state->time,
            state,
            &state->indices[CLOCK_DIGIT_INDEX_SECONDS]);
    //get_indices(INDEX_MINUTES, &state->time, &minutes_indices);
    //get_indices(INDEX_HOURS, &state->time, &hours_indices);

    const clock_digit_indices_s * const seconds_ind =
            &state->indices[CLOCK_DIGIT_INDEX_SECONDS];

    blend_digits(
            state->time.ticks % CLOCK_TICK_PER_LED,
            &state->digits[seconds_ind->cur],
            &state->digits[seconds_ind->next]);







    // first set for seconds, then blend minute/hour

    /*
    const uint16_t seconds = (state->ticks / CLOCK_TICK_PER_SEC);
    const uint16_t index = (seconds / CLOCK_SEC_PER_LED);
    assert(index < CLOCK_DIGIT_LED_COUNT);

    if(index == state->next_index)
    {
        // transition digits

        // this is just for testing, bad index scheme
        const uint16_t prev_index = (index - 1) % CLOCK_DIGIT_LED_COUNT;

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
    */

    /*
    state->time.ticks += 1;

    if(state->time.ticks >= CLOCK_TICKS_PER_REVOLUTION)
    {
        state->time.ticks = 0;
    }
    */
}
