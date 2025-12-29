#ifndef NB_RUNTIME_H
#define NB_RUNTIME_H

#include "nb_context.h"
#include "nb_timer.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __cplusplus
extern "C" {
#endif

#define QUEUE_EVENT_ESTABLISHED (0)
#define QUEUE_EVENT_READ_READY (1)
#define QUEUE_EVENT_ACCEPT_READY (2)

static inline int nb__connection_t_size(void) {
  return sizeof(nb__connection_t);
}

static inline void nb__assert(int b, const char *error) {
  if (!b) {
    fprintf(stderr, "%s\n", error);
    abort();
  }
}

// Routing table
struct routing_table_entry {
  unsigned int global_id;
  unsigned long long local_id;
};
unsigned long long
nb__routing_table_lookup_from_global(unsigned int, struct routing_table_entry *,
                                     int routing_table_size);
unsigned short nb__do_ip_checksum(unsigned char *, size_t len);

// Data queue methods
struct data_queue_t *nb__new_data_queue(void);
void nb__free_data_queue(struct data_queue_t *);
void nb__insert_data_queue(struct data_queue_t *, char *, int);

// Accept queue methods
nb__accept_queue_t *nb__new_accept_queue(void);
void nb__free_accept_queue(nb__accept_queue_t *);
void nb__insert_accept_queue(nb__accept_queue_t *, unsigned, unsigned long long,
                             void *);

void nb__add_connection(nb__connection_t *, unsigned sa);
void nb__delete_connection(unsigned sa);
nb__connection_t *nb__retrieve_connection(unsigned sa);

void nb__debug_packet(char *p, size_t len);

// Network interface API
char *nb__poll_packet(int *, int);
char *nb__poll_packet_ctx(nb__context_t *ctx, int *, int);
int nb__send_packet(char *, int);
int nb__send_packet_ctx(nb__context_t *ctx, char *, int);
void nb__transport_default_init(void);
void nb__transport_default_deinit(void);
char *nb__request_send_buffer(void);
void *nb__return_send_buffer(char *);

// Transport specific API
void nb__ipc_init(const char *sock_path, int mode);
void nb__ipc_deinit();
void nb__mlx5_init(const char *name);

char *nb__request_send_buffer(void);
void *nb__return_send_buffer(char *);
void nb__linux_runtime_init(const char *iface);
void nb__linux_runtime_init_ctx(nb__context_t *ctx, const char *iface);
void nb__linux_runtime_deinit(void);
void nb__linux_runtime_deinit_ctx(nb__context_t *ctx);

// Generated protocol API (global versions)
void nb__run_ingress_step(void *, int);
int nb__send(nb__connection_t *arg0, char *arg1, int arg2);
void nb__destablish(nb__connection_t *arg0);
nb__connection_t *nb__establish(unsigned int arg0, unsigned int arg1,
                                unsigned int arg2,
                                void (*arg3)(int, nb__connection_t *));
void nb__net_init(void);
void nb__reliable_redelivery_timer_cb(nb__timer *, void *param,
                                      unsigned long long to);

// Generated protocol API (context-based versions)
void nb__run_ingress_step_ctx(nb__context_t *ctx, void *, int);
int nb__send_ctx(nb__context_t *ctx, nb__connection_t *arg0, char *arg1,
                 int arg2);
void nb__destablish_ctx(nb__context_t *ctx, nb__connection_t *arg0);
nb__connection_t *nb__establish_ctx(nb__context_t *ctx, unsigned int arg0,
                                    unsigned int arg1, unsigned int arg2,
                                    void (*arg3)(int, nb__connection_t *));
void nb__net_init_ctx(nb__context_t *ctx);

// Runtime API
int nb__read(nb__connection_t *, char *, int);
nb__connection_t *nb__accept(nb__connection_t *,
                             void (*)(int, nb__connection_t *));
nb__connection_t *nb__accept_ctx(nb__context_t *ctx, nb__connection_t *,
                                 void (*)(int, nb__connection_t *));

void nb__main_loop_step(void);
void nb__main_loop_step_ctx(nb__context_t *ctx);

void nb__set_user_data(nb__connection_t *, void *user_data);
void *nb__get_user_data(nb__connection_t *);

extern char nb__reuse_mtu_buffer[];
extern unsigned int nb__my_host_id;
extern unsigned long long nb__my_local_host_id;
extern nb__net_state_t *nb__net_state;
extern unsigned long long nb__wildcard_host_identifier;

extern unsigned long long nb__get_time_ms_now(void);

#define nb__info_log(str, ...)                                                 \
  fprintf(stderr, "INFO [%s]: " str "\n", __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif
