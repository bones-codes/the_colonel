#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(void) {
	int socket_desc;
	struct sockaddr_in server;
	char *message, server_reply[2000];

	/* create socket */
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == socket_desc) {
		printf("ERROR: Could not create socket");
	}

	server.sin_addr.s_addr = inet_addr("74.125.235.20");
	server.sin_family = AF_INET;
	server.sin_port = htons(80);

	/* connect to remote server */
	if (connect(socket_desc,
		(struct sockaddr *)&server, sizeof(server)) < 0) {
		puts("ERROR: Connect error\n");
		return 1;
	}

	puts("Connected\n");

	/* send data */
	message = "GET / HTTP/1.1\r\n\r\n";
	if (send(socket_desc, message, strlen(message), 0) < 0) {
		puts("ERROR: Send failed\n");
		return 1;
	}
	puts("Data sent\n");

	/* receive a reply from the server */
	if (recv(socket_desc, server_reply, 2000, 0) < 0) {
		puts("ERROR: recv failed\n");
	}
	puts("Reply received\n");
	puts(server_reply);

	close(socket_desc);
	return 0;
}