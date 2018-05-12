/**
 * @file clock.c
 * @brief TODO.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "default_config.h"
#include "gl_headers.h"
#include "gui_util.h"
#include "render.h"
#include "clock.h"

static void render_base(
        const clock_s * const clock)
{
    glColor4d(0.39, 0.21, 0.02, 1.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    render_rectangle_2d(0, 0, clock->base_w, clock->base_h);
}

static void render_light(
        const GLdouble cx,
        const GLdouble cy,
        const clock_s * const clock)
{
    glColor4d(1.0, 1.0, 1.0, 0.6);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    render_circle_2d(cx, cy, CLOCK_LIGHT_RADIUS, 1);

    glLineWidth(3.0f);
    glColor4d(0, 0, 0, 1.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    render_circle_2d(cx, cy, CLOCK_LIGHT_RADIUS, 0);
}

static void render_light_assembly(
        const clock_s * const clock)
{
    const double step = (360.0 / 12);

    double angle;
    for(angle = 0; angle < 360.0; angle += step)
    {
        const GLdouble x = CLOCK_RADIUS * cos(RAD(angle));
        const GLdouble y = CLOCK_RADIUS * sin(RAD(angle));

        render_light(x, y, clock);
    }
}

void clock_init(
        clock_s * const clock)
{
    clock->base_w = CLOCK_WIDTH;
    clock->base_h = CLOCK_HEIGHT;
}

void clock_display(
        const clock_s * const clock)
{
    render_base(clock);

    render_light_assembly(clock);
}
