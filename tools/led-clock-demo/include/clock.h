/**
 * @file clock.h
 * @brief TODO.
 *
 */

#ifndef CLOCK_H
#define CLOCK_H

#include "gui_types.h"

// units: millimeter
#define CLOCK_WIDTH (140.0)
#define CLOCK_HEIGHT (140.0)

#define CLOCK_RADIUS (50.0)
#define CLOCK_LIGHT_RADIUS (3.175)

#define CLOCK_LIGHT_COUNT (12)

typedef struct
{
    color_4d_s color;
    point_2d_s pos;
    unsigned int state;
} clock_light_s;

typedef struct
{
    double base_w;
    double base_h;
    unsigned long long ticks;
    clock_light_s lights[CLOCK_LIGHT_COUNT];
} clock_s;

void clock_init(
        clock_s * const clock);

void clock_display(
        const clock_s * const clock);

void clock_tick_inc(
        clock_s * const clock);

#endif /* CLOCK_H */
