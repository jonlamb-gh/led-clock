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

// TODO - move into a .c/.h, methods for init, conversion, blending, etc ?
typedef struct
{
    uint16_t rgb[3];
} clock_digit_led_s;

typedef struct
{
    uint16_t ticks; // reset every 60 seconds
    uint8_t enabled;
    clock_digit_led_s digits[CLOCK_DIGIT_LED_COUNT];
} clock_state_s;

void clock_state_init(
        clock_state_s * const state);

void clock_state_tick(
        clock_state_s * const state);

#endif /* CLOCK_STATE_H */
