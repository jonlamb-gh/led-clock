/**
 * @file gui_types.h
 * @brief TODO.
 *
 */

#ifndef GUI_TYPES_H
#define GUI_TYPES_H

#include "gl_headers.h"

typedef struct
{
    double rgba[4];
} color_4d_s;

typedef struct
{
    int id;
    unsigned long width;
    unsigned long height;
    char title[256];
} gui_window_s;

typedef struct
{
    int gl_argc;
    char **gl_argv;
    GLenum gl_err;
    gui_window_s window;
} gui_s;

#endif /* GUI_TYPES_H */
