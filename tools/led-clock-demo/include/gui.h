/**
 * @file gui.h
 * @brief TODO.
 *
 */

#ifndef GUI_H
#define GUI_H

int gui_init(
        const char * const win_title,
        const unsigned long width,
        const unsigned long height);

void gui_fini(void);

int gui_is_window_closed(void);

void gui_display(void);

void gui_clock_tick_inc(void);

#endif /* GUI_H */
