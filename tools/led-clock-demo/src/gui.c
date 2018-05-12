/**
 * @file gui.c
 * @brief TODO.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "default_config.h"
#include "gl_headers.h"
#include "gui_types.h"
#include "gui_util.h"
#include "gui.h"

static gui_s g_gui;

int gui_init(
        const char * const win_title,
        const unsigned long width,
        const unsigned long height)
{
    int ret;

    // TODO
    ret = 0;

    return ret;
}

void gui_fini(void)
{
    // TODO
    (void) g_gui;
}

void gui_render(void)
{
    // TODO
}
