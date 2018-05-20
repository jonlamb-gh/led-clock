/**
 * @file clock_state.h
 * @brief TODO.
 *
 */

#ifndef CLOCK_STATE_H
#define CLOCK_STATE_H

#include <stdint.h>

#define CLOCK_DIGIT_LED_COUNT (12UL)
#define CLOCK_TICK_PER_SEC (10UL)
#define CLOCK_SEC_PER_LED (5UL)
#define CLOCK_TICK_PER_LED (CLOCK_TICK_PER_SEC * CLOCK_SEC_PER_LED)
#define CLOCK_TICKS_PER_REVOLUTION (CLOCK_TICK_PER_SEC * 60UL)

#define CLOCK_PWM_RESOLUTION (UINT16_MAX)

#define CLOCK_PWM_DISABLED (0)
//#define CLOCK_PWM_ENABLED (UINT16_MAX/2)

typedef enum
{
    CLOCK_DIGIT_INDEX_SECONDS = 0,
    CLOCK_DIGIT_INDEX_MINUTES,
    CLOCK_DIGIT_INDEX_HOURS,
    CLOCK_DIGIT_INDEX_KIND_COUNT
} clock_digit_index_kind;

typedef struct
{
    uint16_t rgb[3];
} clock_digit_led_s;

// maybe
typedef struct
{
    uint16_t ticks; // reset every 60 seconds
    uint16_t seconds;
    uint16_t minutes;
    uint16_t hours;
} clock_time_s;

typedef struct
{
    uint16_t prev;
    uint16_t cur;
    uint16_t next;
    uint8_t color_channel;
} clock_digit_indices_s;

typedef struct
{
    uint8_t enabled;
    clock_time_s time;
    clock_digit_indices_s indices[CLOCK_DIGIT_INDEX_KIND_COUNT];
    clock_digit_led_s digits[CLOCK_DIGIT_LED_COUNT];
} clock_state_s;

void clock_state_init(
        clock_state_s * const state);

void clock_state_tick(
        clock_state_s * const state);

#endif /* CLOCK_STATE_H */
