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
#include "clock_state.h"
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
    for(idx = 0; idx < CLOCK_DIGIT_LED_COUNT; idx += 1)
    {
        render_light(
                &clock->lights[idx]);
    }
}

static void convert_led_digit_color(
        const clock_digit_led_s * const led,
        clock_light_s * const light)
{
    if((led->rgb[0] == 0) && (led->rgb[1] == 0) && (led->rgb[2] == 0))
    {
        light->state = 0;

        light->color.rgba[0] = 0.0;
        light->color.rgba[1] = 0.0;
        light->color.rgba[2] = 0.0;
        light->color.rgba[3] = 0.0;
    }
    else
    {
        light->state = 1;

        light->color.rgba[0] = ((GLdouble) led->rgb[0] / (GLdouble) CLOCK_PWM_RESOLUTION);
        light->color.rgba[1] = ((GLdouble) led->rgb[1] / (GLdouble) CLOCK_PWM_RESOLUTION);
        light->color.rgba[2] = ((GLdouble) led->rgb[2] / (GLdouble) CLOCK_PWM_RESOLUTION);

        // TODO
        light->color.rgba[3] = (light->color.rgba[0] + light->color.rgba[1] + light->color.rgba[2]);
        light->color.rgba[3] = (light->color.rgba[3] / 3);

        light->color.rgba[3] = CONSTRAIN(light->color.rgba[3], 0.6, 1.0);
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

static void init_lights(
        clock_s * const clock)
{
    const GLdouble step = (360.0 / 12);

    unsigned long idx;
    for(idx = 0; idx < CLOCK_DIGIT_LED_COUNT; idx += 1)
    {
        init_clock_light(
                -(((GLdouble) idx) * step) + 90.0,
                &clock->lights[idx]);
    }
}

void clock_init(
        clock_s * const clock)
{
    clock->base_w = CLOCK_WIDTH;
    clock->base_h = CLOCK_HEIGHT;

    init_lights(clock);

    clock_state_init(&clock->state);
}

void clock_display(
        const clock_s * const clock)
{
    // TODO - pwm -> rgba update GUI state

    render_base(clock);

    render_light_assembly(clock);
}

void clock_tick_inc(
        clock_s * const clock)
{
    clock_state_tick(&clock->state);

    // move this to render?
    unsigned long idx;
    for(idx = 0; idx < CLOCK_DIGIT_LED_COUNT; idx += 1)
    {
        convert_led_digit_color(
                &clock->state.digits[idx],
                &clock->lights[idx]);
    }

    /*
    const unsigned long seconds = (clock->ticks / CLOCK_TICK_PER_SEC);

    const unsigned long index = (seconds / 5);

    printf("ticks: %lu\n", clock->ticks);
    printf("seconds: %lu\n", seconds);
    printf("index: %lu\n", index);
    printf("\n");

    //const GLdouble angle = (360.0 / 60.0) * (GLdouble) seconds;

    clock->lights[index].state = 1;
    clock->lights[index].color.rgba[0] = 0.0;
    clock->lights[index].color.rgba[1] = 0.0;
    clock->lights[index].color.rgba[2] += 0.02;
    clock->lights[index].color.rgba[3] += 0.02;

    // reset ticks
    clock->ticks += 1;

    if(clock->ticks >= (CLOCK_TICK_PER_SEC * 60))
    {
        clock->ticks = 0;
        init_lights(clock);
    }
    */

/*
    unsigned long idx;
    for(idx = 0; idx < CLOCK_DIGIT_LED_COUNT; idx += 1)
    {
        clock->lights[idx].state += 1;

        if(clock->lights[idx].state < 255)
        {
            clock->lights[idx].color.rgba[0] += 0.01;
            clock->lights[idx].color.rgba[3] += 0.01;
        }
        else if(clock->lights[idx].state == 255)
        {
            clock->lights[idx].color.rgba[1] = 0.0;
            clock->lights[idx].color.rgba[3] = 0.0;
        }
        else if(clock->lights[idx].state < 510)
        {
            clock->lights[idx].color.rgba[1] += 0.01;
            clock->lights[idx].color.rgba[3] += 0.01;
        }
        else if(clock->lights[idx].state > 765)
        {
            clock->lights[idx].state = 0;
            clock->lights[idx].color.rgba[0] = 0.0;
            clock->lights[idx].color.rgba[1] = 0.0;
            clock->lights[idx].color.rgba[2] = 0.0;
            clock->lights[idx].color.rgba[3] = 0.0;
        }
    }
*/
}
