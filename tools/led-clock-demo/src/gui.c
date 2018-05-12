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
#include "clock.h"
#include "gui.h"

typedef struct
{
    int gl_argc;
    char **gl_argv;
    GLenum gl_err;
    gui_window_s window;
    clock_s clock;
} gui_s;

static gui_s g_gui;

static void gl_close_func(void)
{
    g_gui.window.id = -1;
}

static void gl_key_func(
        unsigned char key,
        int x,
        int y)
{
    if((key == '\e') || (key == 'q'))
    {
        gl_close_func();
    }
}

static void gl_reshape_func(
        int w,
        int h)
{
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    /*
    const double dw = (double) w;
    const double dh = (double) h;
    */

    const double dw = CLOCK_WIDTH + GUI_WINDOW_BORDER_SIZE;
    const double dh = CLOCK_HEIGHT + GUI_WINDOW_BORDER_SIZE;
    gluOrtho2D(-dw/2.0, dw/2.0, -dh/2.0, dh/2.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    g_gui.window.width = (unsigned long) w;
    g_gui.window.height = (unsigned long) h;

    glutPostRedisplay();
}

static void gl_display_func(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 1.0);

    glColor4d(1.0, 1.0, 1.0, 1.0);
    glLineWidth((GLfloat) GUI_DEFAULT_LINE_WIDTH);

    glPointSize(1.0f);

    glEnable(GL_BLEND);

    clock_display(&g_gui.clock);

    glutSwapBuffers();
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
        clock_init(&g_gui.clock);
    }

    if(ret == 0)
    {
        glutCloseFunc(gl_close_func);
        glutReshapeFunc(gl_reshape_func);
        glutDisplayFunc(gl_display_func);
        glutKeyboardFunc(gl_key_func);

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

void gui_display(void)
{
    if(g_gui.window.id >= 0)
    {
        glutPostRedisplay();

        glutMainLoopEvent();
    }
}

void gui_clock_tick_inc(void)
{
    clock_tick_inc(&g_gui.clock);
}
