#ifndef NB_TIMER_IMPL_H
#define NB_TIMER_IMPL_H

#define MAX_TIMER_ALLOCS (1024)

// Forward declaration for context (only pointer needed for function params)
struct nb__context;
typedef struct nb__context nb__context_t;

// Timer callback type forward declaration
struct nb__timer_obj;
typedef void (*nb__timer_callback_t)(struct nb__timer_obj *, void *,
                                     unsigned long long);

// Full definition of timer struct (needed for arrays)
struct nb__timer_obj {
  nb__timer_callback_t callback;
  void *argument;

  // For chaining
  struct nb__timer_obj *next;
  struct nb__timer_obj *prev;
  unsigned long long timeout;

  // Context pointer for context-aware callbacks
  nb__context_t *context;
};

typedef struct nb__timer_obj nb__timer;

// Global timer variables
extern nb__timer nb__allocated_timers[MAX_TIMER_ALLOCS];
extern nb__timer *nb__timer_free_list;

// Global timer functions
extern nb__timer *nb__alloc_timer(void);
extern void nb__return_timer(nb__timer *);
void nb__insert_timer(nb__timer *t, unsigned long long to,
                      nb__timer_callback_t cb, void *argument);
extern void nb__remove_timer(nb__timer *);
extern void nb__init_timers(void);
extern void nb__check_timers(void);

// Context-based timer functions
extern nb__timer *nb__alloc_timer_ctx(nb__context_t *);
extern void nb__return_timer_ctx(nb__context_t *, nb__timer *);
void nb__insert_timer_ctx(nb__context_t *ctx, nb__timer *t,
                          unsigned long long to, nb__timer_callback_t cb,
                          void *argument);
extern void nb__remove_timer_ctx(nb__context_t *, nb__timer *);
extern void nb__init_timers_ctx(nb__context_t *);
extern void nb__check_timers_ctx(nb__context_t *);

// Helper to get context from timer (for callbacks)
static inline nb__context_t *nb__get_timer_context(nb__timer *t) {
  return t->context;
}

#endif
