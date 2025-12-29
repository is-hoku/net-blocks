#include "nb_runtime.h"
#include <string.h>

nb__context_t *nb__context_create(void) {
  nb__context_t *ctx = (nb__context_t *)malloc(sizeof(nb__context_t));
  if (ctx == NULL) {
    return NULL;
  }
  // Zero-initialize the entire context structure
  memset(ctx, 0, sizeof(nb__context_t));

  // Allocate net_state for this context
  ctx->nb__net_state = (nb__net_state_t *)malloc(sizeof(nb__net_state_t));
  if (ctx->nb__net_state == NULL) {
    free(ctx);
    return NULL;
  }
  // Zero-initialize the net_state structure
  memset(ctx->nb__net_state, 0, sizeof(nb__net_state_t));

  ctx->nb__time_now = 0;
  ctx->nb__my_host_id = 0;
  ctx->nb__my_local_host_id = 0;
  ctx->nb__wildcard_host_identifier = 0;
  ctx->main_socket = 0;

  // Initialize timers for this context
  nb__init_timers_ctx(ctx);

  return ctx;
}

void nb__context_destroy(nb__context_t *ctx) {
  if (ctx) {
    if (ctx->nb__net_state) {
      free(ctx->nb__net_state);
    }
    free(ctx);
  }
}
