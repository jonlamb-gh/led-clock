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

static uint8_t increment_indices(
        clock_digit_indices_s * const indices)
{
    uint8_t did_transition = 0;

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

static uint8_t update_hours_indices(
        const clock_time_s * const time,
        clock_digit_indices_s * const indices)
{
    indices->cur = (time->hours / CLOCK_HOURS_PER_LED);
    assert(indices->cur < CLOCK_DIGIT_LED_COUNT);

    return increment_indices(indices);
}

static uint8_t update_minutes_indices(
        const clock_time_s * const time,
        clock_digit_indices_s * const indices)
{
    // TODO - this is wrong
    indices->cur = (time->minutes / CLOCK_MINUTES_PER_LED);
    assert(indices->cur < CLOCK_DIGIT_LED_COUNT);

    return increment_indices(indices);
}

static uint8_t update_seconds_indices(
        const clock_time_s * const time,
        clock_digit_indices_s * const indices)
{
    indices->cur = (time->seconds / CLOCK_SECONDS_PER_LED);
    assert(indices->cur < CLOCK_DIGIT_LED_COUNT);

    return increment_indices(indices);
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
    else if(kind == CLOCK_DIGIT_INDEX_MINUTES)
    {
        did_transition = update_minutes_indices(time, indices);
    }
    else if(kind == CLOCK_DIGIT_INDEX_HOURS)
    {
        did_transition = update_hours_indices(time, indices);
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
        const clock_digit_index_kind kind,
        const clock_time_s * const time,
        const uint8_t color_channel,
        clock_digit_led_s * const cur_led,
        clock_digit_led_s * const next_led)
{
    uint16_t delta = 0;
    const uint16_t max_duty = CLOCK_PWM_RESOLUTION;

    if(kind == CLOCK_DIGIT_INDEX_SECONDS)
    {
        const uint16_t duty_per_tick = (max_duty / CLOCK_TICKS_PER_LED);
        delta = (time->ticks % CLOCK_TICKS_PER_LED) * duty_per_tick;
    }
    else if(kind == CLOCK_DIGIT_INDEX_MINUTES)
    {
        const uint16_t duty_per_min = (max_duty / CLOCK_MINUTES_PER_LED);
        const uint16_t duty_per_sec = (duty_per_min / CLOCK_SECONDS_PER_MINUTE);

        delta = (time->minutes % CLOCK_MINUTES_PER_LED) * duty_per_min;
        delta += ((time->seconds % CLOCK_SECONDS_PER_MINUTE) * duty_per_sec);
    }
    else if(kind == CLOCK_DIGIT_INDEX_HOURS)
    {
        // no blending of the hour
        //const uint16_t duty_per_min = (max_duty / CLOCK_MINUTES_PER_HOUR);
        //delta = (time->minutes % CLOCK_MINUTES_PER_HOUR) * duty_per_min;
        delta = 0;
    }

    // TODO - sanity checks
    cur_led->rgb[color_channel] = CLOCK_PWM_RESOLUTION - delta;

    next_led->rgb[color_channel] = delta;
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

        // TESTING - time is normally provided
        state->time.minutes += 1;

        if(state->time.minutes >= 60)
        {
            state->time.minutes = 0;
            state->time.hours += 1;
        }
    }

    state->time.seconds = (state->time.ticks / CLOCK_TICKS_PER_SEC);

    // TODO - make this better, this is bad
    update_indices(
            CLOCK_DIGIT_INDEX_SECONDS,
            &state->time,
            state,
            &state->indices[CLOCK_DIGIT_INDEX_SECONDS]);

    update_indices(
            CLOCK_DIGIT_INDEX_MINUTES,
            &state->time,
            state,
            &state->indices[CLOCK_DIGIT_INDEX_MINUTES]);

    update_indices(
            CLOCK_DIGIT_INDEX_HOURS,
            &state->time,
            state,
            &state->indices[CLOCK_DIGIT_INDEX_HOURS]);

    const clock_digit_indices_s * const seconds_ind =
            &state->indices[CLOCK_DIGIT_INDEX_SECONDS];

    blend_digits(
            CLOCK_DIGIT_INDEX_SECONDS,
            &state->time,
            state->indices[CLOCK_DIGIT_INDEX_SECONDS].color_channel,
            &state->digits[seconds_ind->cur],
            &state->digits[seconds_ind->next]);

    const clock_digit_indices_s * const minutes_ind =
            &state->indices[CLOCK_DIGIT_INDEX_MINUTES];

    blend_digits(
            CLOCK_DIGIT_INDEX_MINUTES,
            &state->time,
            state->indices[CLOCK_DIGIT_INDEX_MINUTES].color_channel,
            &state->digits[minutes_ind->cur],
            &state->digits[minutes_ind->next]);

    const clock_digit_indices_s * const hours_ind =
            &state->indices[CLOCK_DIGIT_INDEX_HOURS];

    blend_digits(
            CLOCK_DIGIT_INDEX_HOURS,
            &state->time,
            state->indices[CLOCK_DIGIT_INDEX_HOURS].color_channel,
            &state->digits[hours_ind->cur],
            &state->digits[hours_ind->next]);
}
