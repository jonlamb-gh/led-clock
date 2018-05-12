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

static void gl_close_func(void)
{
    g_gui.window.id = -1;
}

int gui_init(
        const char * const win_title,
        const unsigned long width,
        const unsigned long height)
{
    int ret;

    (void) memset(&g_gui, 0, sizeof(g_gui));

    (void) strncpy(
            g_gui.window.title,
            win_title,
            sizeof(g_gui.window.title));
    g_gui.window.width = width;
    g_gui.window.height = height;

    glutInit(&g_gui.gl_argc, g_gui.gl_argv);

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

    glutInitWindowSize(g_gui.window.width, g_gui.window.height);

    g_gui.window.id = glutCreateWindow(g_gui.window.title);

    if(g_gui.window.id < 0)
    {
        ret = 1;
    }

    if(ret == 0)
    {
        glutCloseFunc(gl_close_func);
        /*
        glutKeyboardFunc(on_key);
        glutSpecialFunc(on_special_key);
        glutMouseFunc(on_mouse);
        glutMotionFunc(on_mouse_motion);
        glutReshapeFunc(on_resize);
        glutDisplayFunc(on_draw);
        */

        glDisable(GL_DEPTH);
        glDisable(GL_LIGHTING);
        glShadeModel(GL_SMOOTH);
        glDisable(GL_DEPTH_TEST);

        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_POLYGON_SMOOTH);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // main loop returns on window exit
        glutSetOption(
                GLUT_ACTION_ON_WINDOW_CLOSE,
                GLUT_ACTION_GLUTMAINLOOP_RETURNS);

        // clear the color buffer, background, to black, RGBA
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        // signal redraw
        glutPostRedisplay();

        // process glut events
        glutMainLoopEvent();
    }

    return ret;
}

void gui_fini(void)
{
    if(g_gui.window.id >= 0)
    {
        glutExit();
    }
}

int gui_is_window_closed(void)
{
    int ret;

    if(g_gui.window.id >= 0)
    {
        ret = 0;
    }
    else
    {
        ret = 1;
    }

    return ret;
}

void gui_render(void)
{
    if(g_gui.window.id >= 0)
    {
        glutPostRedisplay();

        glutMainLoopEvent();
    }
}
