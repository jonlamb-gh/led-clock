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
        const clock_light_s * const light)
{
    glColor4d(1.0, 1.0, 1.0, 0.6);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    render_circle_2d(
            light->pos.xy[0],
            light->pos.xy[1],
            CLOCK_LIGHT_RADIUS,
            1);

    if(light->state != 0)
    {
        glColor4dv(&light->color.rgba[0]);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        render_circle_2d(
                light->pos.xy[0],
                light->pos.xy[1],
                CLOCK_LIGHT_RADIUS,
                1);
    }

    glLineWidth(3.0f);
    glColor4d(0, 0, 0, 1.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    render_circle_2d(
            light->pos.xy[0],
            light->pos.xy[1],
            CLOCK_LIGHT_RADIUS,
            0);
}

static void render_light_assembly(
        const clock_s * const clock)
{
    unsigned long idx;
    for(idx = 0; idx < CLOCK_LIGHT_COUNT; idx += 1)
    {
        render_light(
                &clock->lights[idx]);
    }
}

static void init_clock_light(
        const GLdouble angle,
        clock_light_s * const light)
{
    light->color.rgba[0] = 0.0;
    light->color.rgba[1] = 0.0;
    light->color.rgba[2] = 0.0;
    light->color.rgba[3] = 0.0;

    light->pos.xy[0] = CLOCK_RADIUS * cos(RAD(angle));
    light->pos.xy[1] = CLOCK_RADIUS * sin(RAD(angle));

    light->state = 0;
}

void clock_init(
        clock_s * const clock)
{
    clock->base_w = CLOCK_WIDTH;
    clock->base_h = CLOCK_HEIGHT;

    clock->ticks = 0;

    const GLdouble step = (360.0 / 12);

    unsigned long idx;
    for(idx = 0; idx < CLOCK_LIGHT_COUNT; idx += 1)
    {
        init_clock_light(
                ((GLdouble) idx) * step,
                &clock->lights[idx]);
    }
}

void clock_display(
        const clock_s * const clock)
{
    render_base(clock);

    render_light_assembly(clock);
}

void clock_tick_inc(
        clock_s * const clock)
{
    printf("tick\n");
}
