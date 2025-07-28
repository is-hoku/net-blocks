#include <errno.h>
#include <fcntl.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <stdio.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <ctype.h>
#include <net/if.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#define LINUX_MTU (1024)

static int main_socket = 0;

static int is_netblocks_packet(char *packet, int len) {
  if (len != 16 && len != 38 && len != 39 && len != 43 && len != 294) {
    return 0;
  }
  return 1;
}

void nb__linux_runtime_init(char *interface_name) {
  main_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  if (!main_socket) {
    printf("Failed to create raw socket, are you running as root?\n");
    exit(1);
  }

  struct ifreq req;
  strcpy(req.ifr_name, interface_name);

  if (ioctl(main_socket, SIOCGIFINDEX, &req)) {
    printf("Failed to grab interface id, is the interface name (%s) correct?\n",
           interface_name);
    exit(1);
  }

  struct sockaddr_ll addr;
  addr.sll_family = AF_PACKET;
  addr.sll_protocol = htons(ETH_P_ALL);
  addr.sll_ifindex = req.ifr_ifindex;

  if (bind(main_socket, (struct sockaddr *)&addr, sizeof(addr))) {
    printf("Failed to bind to raw socket\n");
    exit(1);
  }
  fcntl(main_socket, F_SETFL, O_NONBLOCK);
  return;
}

void nb__linux_runtime_deinit(void) { close(main_socket); }

// All nb runtime functions

char *nb__poll_packet(int *size, int headroom) {
  int len;
  static char temp_buf[LINUX_MTU];
  *size = 0;

  while (1) {
    len = (int)read(main_socket, temp_buf, LINUX_MTU);
    if (len <= 0) {
      return NULL; // No more packets or error
    }

    // Apply NetBlocks packet filter
    if (!is_netblocks_packet(temp_buf, len)) {
      continue;
    }

    char *buf = malloc(LINUX_MTU + headroom);
    memcpy(buf + headroom, temp_buf, LINUX_MTU);
    *size = len;
    return buf;
  }
}

int nb__send_packet(char *buff, int len) {
  int ret = write(main_socket, buff, len);
  return ret;
}

char *nb__request_send_buffer(void) {
  // TODO: This extra space needs to come from the headroom, either directly
  // call get_headroom or get it as a parameter
  char *buffer = malloc(LINUX_MTU + 32);
  if (buffer) {
    memset(buffer, 0, LINUX_MTU + 32);
  }
  return buffer;
}

void *nb__return_send_buffer(char *p) { free(p); }
