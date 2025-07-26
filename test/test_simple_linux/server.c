#include "nb_runtime.h"
#include <unistd.h>
#include <stdio.h>

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

#define SERVER_MSG ("Hello from server")

char client_id[] = {0x00, 0x15, 0x5d, 0xe7, 0x9f, 0xb4};
char server_id[] = {0x00, 0x15, 0x5d, 0xe7, 0x9f, 0xb5};

int running = 1;
static void callback(int event, nb__connection_t * c) {
	if (event == QUEUE_EVENT_READ_READY) {
		char buff[65];
		int len = nb__read(c, buff, 64);
		buff[len] = 0;	

		printf("SERVER RECEIVED DATA: %s\n", buff);
		print_packet_bytes("SERVER_READ", buff, len);
		
		printf("SERVER SENDING: %s\n", SERVER_MSG);
		print_packet_bytes("SERVER_SEND", SERVER_MSG, sizeof(SERVER_MSG));
		nb__send(c, SERVER_MSG, sizeof(SERVER_MSG));
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

	nb__my_host_id = server_id_i;

	nb__net_init();

	//nb__connection_t * conn = nb__establish(client_id_i, 8081, 8080, callback);
	nb__connection_t * conn = nb__establish(nb__wildcard_host_identifier, 0, 8080, callback);

	while (running) {
		nb__main_loop_step();
		usleep(100 * 1000);
	}
	nb__destablish(conn);
	return 0;
}
