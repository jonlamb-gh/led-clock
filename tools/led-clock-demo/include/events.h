/**
 * @file events.h
 * @brief TODO.
 *
 */

#ifndef EVENTS_H
#define EVENTS_H

#include <stdint.h>

enum events_e
{
    EVENTS_NONE = 0,
    EVENTS_CLOCK_TICK = (1 << 0),
    EVENTS_GUI_REDRAW = (1 << 1)
};

typedef void (*events_broadcast_fx)(
        const uint32_t event_flags,
        volatile uint32_t * const events);

typedef struct
{
    volatile uint32_t events;
    events_broadcast_fx broadcast_fx;
} events_context_s;

static inline void events_init(
        events_broadcast_fx broadcast_fx,
        events_context_s * const ctx)
{
    ctx->events = EVENTS_NONE;
    ctx->broadcast_fx = broadcast_fx;
}

static inline uint32_t events_get_and_clear(
        events_context_s * const ctx)
{
    const volatile uint32_t m_events = ctx->events;

    ctx->events &= ~m_events;

    return (uint32_t) m_events;
}

static inline void events_broadcast(
        const uint32_t event_flags,
        events_context_s * const ctx)
{
    ctx->broadcast_fx(event_flags, &ctx->events);
}

#endif /* EVENTS_H */
