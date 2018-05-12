/**
 * @file render.h
 * @brief TODO.
 *
 */

#ifndef RENDER_H
#define RENDER_H

#include "gl_headers.h"

void render_rectangle_2d(
        const GLdouble cx,
        const GLdouble cy,
        const GLdouble length,
        const GLdouble width);

void render_circle_2d(
        const GLdouble cx,
        const GLdouble cy,
        const GLdouble radius,
        const GLuint fill);

#endif /* RENDER_H */
