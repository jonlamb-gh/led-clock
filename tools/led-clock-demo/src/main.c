/**
 * @file main.c
 * @brief Main.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <popt.h>

#include "default_config.h"
#include "atimer.h"
#include "events.h"
#include "gui_types.h"
#include "gui.h"

enum option_e
{
    OPTION_VERBOSE = 1,
    OPTION_GUI_WIDTH,
    OPTION_GUI_HEIGHT,
    OPTION_GUI_REDRAW_INTERVAL,
    OPTION_CLOCK_TICK_INTERVAL
};

static volatile sig_atomic_t g_exit_signaled;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static void sig_handler(
        int sig)
{
    if(sig == SIGINT)
    {
        g_exit_signaled = 1;
    }
}

static void broadcast_events(
        const uint32_t event_flags,
        volatile uint32_t * const events)
{
    int status;

    status = pthread_mutex_lock(&mutex);

    if(status == 0)
    {
        *events |= event_flags;

        status = pthread_cond_signal(&cond);
    }

    if(status == 0)
    {
        status = pthread_mutex_unlock(&mutex);
    }

    if(status != 0)
    {
        exit(1);
    }
}

static void gui_redraw_timer_callback(
        union sigval data)
{
    if(data.sival_ptr != NULL)
    {
        events_broadcast(
                EVENTS_GUI_REDRAW,
                (events_context_s*) data.sival_ptr);
    }
}

static void clock_tick_timer_callback(
        union sigval data)
{
    if(data.sival_ptr != NULL)
    {
        events_broadcast(
                EVENTS_CLOCK_TICK,
                (events_context_s*) data.sival_ptr);
    }
}

static void opt_exit(
        poptContext opt_ctx)
{
    poptPrintUsage(opt_ctx, stderr, 0);
    poptFreeContext(opt_ctx);
    exit(EXIT_FAILURE);
}

int main(
        int argc,
        char **argv)
{
    int ret = 0;
    int opt_verbose = 0;
    long opt_gui_redraw_intvl = (long) DEF_GUI_REDRAW_INTERVAL_MS;
    long opt_clock_tick_intvl = (long) DEF_CLOCK_TICK_INTERVAL_MS;
    long opt_gui_width = (long) DEF_GUI_WIDTH;
    long opt_gui_height = (long) DEF_GUI_HEIGHT;
    struct sigaction sigact;
    poptContext opt_ctx;
    events_context_s events_ctx;
    atimer_s gui_redraw_timer;
    atimer_s clock_tick_timer;
    struct itimerspec gui_redraw_timer_spec;
    struct itimerspec clock_tick_timer_spec;

    const struct poptOption OPTIONS_TABLE[] =
    {
        {
            "verbose",
            'v',
            POPT_ARG_NONE,
            NULL,
            OPTION_VERBOSE,
            "enable verbose output",
            NULL
        },
        {
            "width",
            'w',
            POPT_ARG_LONG,
            &opt_gui_width,
            OPTION_GUI_WIDTH,
            "GUI width",
            "1-N <pixels>"
        },
        {
            "height",
            'h',
            POPT_ARG_LONG,
            &opt_gui_height,
            OPTION_GUI_HEIGHT,
            "GUI height",
            "1-N <pixels>"
        },
        {
            "gui-redraw-interval",
            'g',
            POPT_ARG_LONG,
            &opt_gui_redraw_intvl,
            OPTION_GUI_REDRAW_INTERVAL,
            "GUI redraw timer interval",
            "1-N <milliseconds>"
        },
        {
            "clock-tick-interval",
            'c',
            POPT_ARG_LONG,
            &opt_clock_tick_intvl,
            OPTION_CLOCK_TICK_INTERVAL,
            "clock tick timer interval",
            "1-N <milliseconds>"
        },
        POPT_AUTOHELP
        POPT_TABLEEND
    };

    opt_ctx = poptGetContext(
            NULL,
            argc,
            (const char**) argv,
            OPTIONS_TABLE,
            0);

    int opt_ret;
    while((opt_ret = poptGetNextOpt(opt_ctx)) >= 0)
    {
        if(opt_ret == OPTION_VERBOSE)
        {
            opt_verbose = 1;
        }
        else if(opt_ret == OPTION_GUI_WIDTH)
        {
            if(opt_gui_width <= 0)
            {
                (void) fprintf(
                        stderr,
                        "GUI width must be greater than zero\n");
                opt_exit(opt_ctx);
            }
        }
        else if(opt_ret == OPTION_GUI_HEIGHT)
        {
            if(opt_gui_height <= 0)
            {
                (void) fprintf(
                        stderr,
                        "GUI height must be greater than zero\n");
                opt_exit(opt_ctx);
            }
        }
        else if(opt_ret == OPTION_GUI_REDRAW_INTERVAL)
        {
            if(opt_gui_redraw_intvl <= 0)
            {
                (void) fprintf(
                        stderr,
                        "GUI redraw timer interval must be greater than zero\n");
                opt_exit(opt_ctx);
            }
        }
        else if(opt_ret == OPTION_CLOCK_TICK_INTERVAL)
        {
            if(opt_clock_tick_intvl <= 0)
            {
                (void) fprintf(
                        stderr,
                        "clock tick timer interval must be greater than zero\n");
                opt_exit(opt_ctx);
            }
        }
    }

    if(opt_ret < -1)
    {
        (void) fprintf(
                stderr,
                "argument error '%s': %s\n\n",
                poptBadOption(opt_ctx, POPT_BADOPTION_NOALIAS),
                poptStrerror(opt_ret));
        poptPrintUsage(opt_ctx, stderr, 0);
        poptFreeContext(opt_ctx);
        exit(EXIT_FAILURE);
    }

    poptFreeContext(opt_ctx);

    g_exit_signaled = 0;
    (void) memset(&sigact, 0, sizeof(sigact));

    sigact.sa_flags = SA_RESTART;
    sigact.sa_handler = sig_handler;

    if(ret == 0)
    {
        ret = sigaction(SIGINT, &sigact, 0);
    }

    (void) memset(&events_ctx, 0, sizeof(events_ctx));

    if(ret == 0)
    {
        events_init(&broadcast_events, &events_ctx);
    }

    if(ret == 0)
    {
        ret = atimer_create(
                gui_redraw_timer_callback,
                (void*) &events_ctx,
                &gui_redraw_timer);
    }

    if(ret == 0)
    {
        atimer_timespec_set_ms(
                DEF_TIMER_START_DELAY_MS,
                &gui_redraw_timer_spec.it_value);
    }

    if(ret == 0)
    {
        atimer_timespec_set_ms(
                (unsigned long) opt_gui_redraw_intvl,
                &gui_redraw_timer_spec.it_interval);
    }

    if(ret == 0)
    {
        ret = atimer_create(
                clock_tick_timer_callback,
                (void*) &events_ctx,
                &clock_tick_timer);
    }

    if(ret == 0)
    {
        atimer_timespec_set_ms(
                DEF_TIMER_START_DELAY_MS,
                &clock_tick_timer_spec.it_value);
    }

    if(ret == 0)
    {
        atimer_timespec_set_ms(
                (unsigned long) opt_clock_tick_intvl,
                &clock_tick_timer_spec.it_interval);
    }

    if(ret == 0)
    {
        ret = gui_init(
                "led-clock-demo",
                (unsigned long) opt_gui_width,
                (unsigned long) opt_gui_height);
    }

    if((opt_verbose != 0) && (ret == 0))
    {
        (void) fprintf(
                stdout,
                "starting timers");
    }

    if(ret == 0)
    {
        ret = atimer_set(
                &gui_redraw_timer_spec,
                &gui_redraw_timer);
    }

    if(ret == 0)
    {
        ret = atimer_set(
                &clock_tick_timer_spec,
                &clock_tick_timer);
    }

    // don't start if we've encountered an error
    if(ret != 0)
    {
        g_exit_signaled = 1;

        (void) fprintf(
                stderr,
                "error detected during initialization\n");
    }

    while((g_exit_signaled == 0) && (ret == 0))
    {
        uint32_t events = EVENTS_NONE;

        ret = pthread_mutex_lock(&mutex);

        if(ret == 0)
        {
            ret = pthread_cond_wait(&cond, &mutex);

            events = events_get_and_clear(&events_ctx);

            ret |= pthread_mutex_unlock(&mutex);
        }

        if((events & EVENTS_CLOCK_TICK) != 0)
        {
            gui_clock_tick_inc();
        }

        const uint32_t redraw_events =
                events & (EVENTS_GUI_REDRAW | EVENTS_CLOCK_TICK);

        if(redraw_events != 0)
        {
            gui_display();
        }

        if(gui_is_window_closed() != 0)
        {
            g_exit_signaled = 1;
        }
    }

    gui_fini();

    (void) atimer_destroy(&gui_redraw_timer);
    (void) atimer_destroy(&clock_tick_timer);

    if(ret == 0)
    {
        ret = EXIT_SUCCESS;
    }
    else
    {
        ret = EXIT_FAILURE;
    }

    return ret;
}
