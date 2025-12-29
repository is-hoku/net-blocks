#ifndef NB_CONTEXT_H
#define NB_CONTEXT_H

#include "gen_headers.h"
#include "nb_timer.h" // Need full nb__timer definition for array

// Context structure definition
struct nb__context {
  // nb__runtime.c
  nb__net_state_t *nb__net_state;
  unsigned long long nb__time_now;
  unsigned int nb__my_host_id;
  unsigned long long nb__my_local_host_id;
  unsigned long long nb__wildcard_host_identifier;
  // nb__linux_transport.c
  int main_socket;
  // nb__timer.c
  nb__timer nb__allocated_timers[MAX_TIMER_ALLOCS];
  nb__timer *nb__timer_free_list;
  nb__timer *nb__timers_head;
  unsigned long long nb__last_timer_checked;
};

// Note: nb__context_t typedef is in nb_timer.h (forward declaration)
// This is the full definition

nb__context_t *nb__context_create(void);
void nb__context_destroy(nb__context_t *ctx);

#endif
