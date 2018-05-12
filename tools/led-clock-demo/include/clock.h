/**
 * @file clock.h
 * @brief TODO.
 *
 */

#ifndef CLOCK_H
#define CLOCK_H

// units: millimeter
#define CLOCK_WIDTH (140.0)
#define CLOCK_HEIGHT (140.0)

#define CLOCK_RADIUS (50.0)
#define CLOCK_LIGHT_RADIUS (3.175)

typedef struct
{
    double base_w;
    double base_h;
} clock_s;

void clock_init(
        clock_s * const clock);

// TODO - update/tick?

void clock_display(
        const clock_s * const clock);

#endif /* CLOCK_H */
