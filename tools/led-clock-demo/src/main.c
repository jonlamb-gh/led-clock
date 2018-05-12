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

#define TIMER_EVENT_GUI_REDRAW (0x01)

enum option_e
{
    OPTION_VERBOSE = 1,
    OPTION_GUI_REDRAW_INTERVAL,
    OPTION_GUI_WIDTH,
    OPTION_GUI_HEIGHT
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
    long opt_gui_width = (long) DEF_GUI_WIDTH;
    long opt_gui_height = (long) DEF_GUI_HEIGHT;
    struct sigaction sigact;
    poptContext opt_ctx;
    events_context_s events_ctx;
    atimer_s gui_redraw_timer;
    struct itimerspec gui_redraw_timer_spec;

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
            "gui-redraw-interval",
            'g',
            POPT_ARG_LONG,
            &opt_gui_redraw_intvl,
            OPTION_GUI_REDRAW_INTERVAL,
            "GUI redraw interval",
            "1-N <milliseconds>"
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
        else if(opt_ret == OPTION_GUI_REDRAW_INTERVAL)
        {
            if(opt_gui_redraw_intvl <= 0)
            {
                (void) fprintf(
                        stderr,
                        "GUI redraw interval must be greater than zero\n");
                opt_exit(opt_ctx);
            }
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

    // enable timers
    if(ret == 0)
    {
        ret = atimer_set(
                &gui_redraw_timer_spec,
                &gui_redraw_timer);
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

        const uint32_t redraw_events =
                events & (EVENTS_GUI_REDRAW | EVENTS_BTN_PRESS | EVENTS_BTN_RELEASE);

        if(redraw_events != 0)
        {
            gui_render();
        }
    }

    gui_fini();

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