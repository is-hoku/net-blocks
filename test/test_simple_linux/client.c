#include "nb_runtime.h"
#include <stdio.h>
#include <unistd.h>

// Function to print packet bytes for debugging
void print_packet_bytes(const char* label, const char* packet, int len) {
    printf("[%s] Packet (%d bytes): ", label, len);
    for (int i = 0; i < len; i++) {
        printf("%02x ", (unsigned char)packet[i]);
        if ((i + 1) % 16 == 0) printf("\n[%s]                   ", label);
    }
    printf("\n");
    fflush(stdout);
}

#define CLIENT_MSG ("Hello from client")

char client_id[] = {0x00, 0x15, 0x5d, 0xe7, 0x9f, 0xb4};
char server_id[] = {0x00, 0x15, 0x5d, 0xe7, 0x9f, 0xb5};

int running = 1;
static void callback(int event, nb__connection_t * c) {
	if (event == QUEUE_EVENT_READ_READY) {
		char buff[65];
		int len = nb__read(c, buff, 64);
		buff[len] = 0;	

		printf("CLIENT RECEIVED DATA: %s\n", buff);
		print_packet_bytes("CLIENT_READ", buff, len);

		running = 0;
	}
}

int main(int argc, char* argv[]) {

	nb__linux_runtime_init("wlp0s20f3");
	printf("Linux Runtime initialized\n");

	unsigned long long server_id_i = 0;
	unsigned long long client_id_i = 0;
	memcpy(&server_id_i, server_id, sizeof(server_id));
	memcpy(&client_id_i, client_id, sizeof(client_id));

	nb__my_host_id = client_id_i;

	nb__net_init();

	nb__connection_t * conn = nb__establish(server_id_i, 8080, 8081, callback);
	
	printf("CLIENT SENDING: %s\n", CLIENT_MSG);
	print_packet_bytes("CLIENT_SEND", CLIENT_MSG, sizeof(CLIENT_MSG));
	nb__send(conn, CLIENT_MSG, sizeof(CLIENT_MSG));

	while (running) {
		nb__main_loop_step();
		usleep(100 * 1000);
	}
	nb__destablish(conn);
	return 0;	
}
