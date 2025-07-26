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
  // Based on actual NetBlocks packet analysis:
  // - Control packets: 38 bytes
  // - ACK packets: 39 bytes
  // - Data packets: 56 bytes (38 header + 18 payload)

  // Only accept known NetBlocks packet sizes
  if (len != 38 && len != 39 && len != 56 && len != 55) {
    printf("[FILTER] Rejecting packet: invalid size %d bytes (expected 38, 39, "
           "56, or 55)\n",
           len);
    return 0;
  }

  // For NetBlocks packets, check if it's our own packet
  // NetBlocks packets start directly with dst_host_id (6 bytes) and src_host_id
  // (6 bytes)
  if (len >= 12) {
    unsigned int src_ip = 0;
    memcpy(&src_ip, packet + 6,
           4); // src_host_id starts at byte 6, get first 4 bytes

    // Get our IP address
    extern unsigned int nb__my_host_id;

    if (src_ip == nb__my_host_id) {
      printf("[FILTER] Rejecting our own packet: src_ip=0x%x, my_ip=0x%x\n",
             src_ip, nb__my_host_id);
      return 0; // Skip our own packets
    }
  }

  printf("[FILTER] Accepting NetBlocks packet: valid size %d bytes\n", len);
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

    // Print all bytes for debugging (including header)
    printf("[nb__poll_packet] Full packet (%d bytes): ", len);
    for (int i = 0; i < len; i++) {
      printf("%02x ", (unsigned char)temp_buf[i]);
      if ((i + 1) % 16 == 0)
        printf("\n[nb__poll_packet]                     ");
    }
    printf("\n");

    // Apply NetBlocks packet filter
    if (!is_netblocks_packet(temp_buf, len)) {
      printf(
          "[nb__poll_packet] Packet filtered out, continuing to next packet\n");
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
