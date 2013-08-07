#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

int main(void) {
	int socket_desc, new_socket, c, client_port;
	char *client_ip;
	struct sockaddr_in server, client;
	char *message;

	/* create socket */
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == socket_desc) {
		printf("ERROR: Could not create socket\n");
	}

	/* prepare the sockaddr_in structure */
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(8888);

	/* bind */
	if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
		puts("ERROR: Bind failed\n");
		return 1;
	}
	puts("bind done");

	/* listening */
	listen(socket_desc, 3);

	/* accept incoming connection */
	puts("waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);
	new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
	if (new_socket < 0) {
		perror("accept failed");
	}

	client_ip = inet_ntoa(client.sin_addr);
	client_port = ntohs(client.sin_port);

	puts("Connection accepted");
	printf("Incoming IP: %s\nPort: %d\n", client_ip, client_port);

	return 0;
}