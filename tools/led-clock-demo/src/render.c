/**
 * @file render.c
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

static GLuint get_circle_segments(
        const GLdouble r)
{
    return (GLuint) (40.0 * sqrt(r));
}

void render_rectangle_2d(
        const GLdouble cx,
        const GLdouble cy,
        const GLdouble length,
        const GLdouble width)
{
    glBegin(GL_QUADS);

    // v0, v1, v2, v3
    glVertex2d(cx - (length/2.0), cy + (width/2.0));
    glVertex2d(cx - (length/2.0), cy - (width/2.0));
    glVertex2d(cx + (length/2.0), cy - (width/2.0));
    glVertex2d(cx + (length/2.0), cy + (width/2.0));

    glEnd();
}

void render_circle_2d(
        const GLdouble cx,
        const GLdouble cy,
        const GLdouble radius,
        const GLuint fill)
{
    // start at angle = 0
    double t = 0.0;
	double x = radius;
	double y = 0.0;

    const GLuint segments = get_circle_segments(radius);

    const double theta = (2.0 * M_PI) / (double) segments;

	const double c = cos(theta);
    const double s = sin(theta);

    if(fill == 0)
    {
	    glBegin(GL_LINE_LOOP);
    }
    else
    {
	    glBegin(GL_POLYGON);
    }

    // repeat rotation with line loop
    GLuint idx;
	for(idx = 0; idx < segments; idx += 1)
	{
		glVertex2d( x + cx, y + cy );

		// apply the rotation matrix
		t = x;
		x = c * x - s * y;
		y = s * t + c * y;
	}

	glEnd();
}
