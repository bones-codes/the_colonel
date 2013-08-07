#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *connection_handler(void *);


int main(void) {
	int socket_desc, new_socket, c, client_port, *new_sock;
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
		puts("ERROR: bind failed\n");
		return 1;
	}
	puts("bind complete");

	/* listening */
	listen(socket_desc, 3);

	/* accept and incoming connection */
	puts("waiting for incoming connections...\n");
	c = sizeof(struct sockaddr_in);	
	while ((new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c))) {
		client_ip = inet_ntoa(client.sin_addr);
		client_port = ntohs(client.sin_port);

		puts("Connection accepted");
		printf("Incoming IP: %s\nPort: %d\n", client_ip, client_port);

		/* reply to client */
		message = "Connection received\n";
		write(new_socket, message, strlen(message));

		pthread_t sniffer_thread;
		new_sock = malloc(1);
		*new_sock = new_socket;
		if (pthread_create(&sniffer_thread, NULL, connection_handler, (void*) new_sock) < 0) {
			perror("ERROR: Could not create thread");
			return 1;
		}

		/* now join the thread so we don't terminate 
		 * before pthread_join(sniffer_thread, NULL); */
		puts("Handler assigned");
	}
	if (new_socket < 0) {
		perror("ERROR: accept failed\n");
		return 1;
	}

	return 0;
}

/* This will handle the connection for each client */
void *connection_handler(void *socket_desc) {
	int sock = *(int*)socket_desc;		/* get the socket descriptor */
	int read_size;
	char *message, client_message[2000];

	message = "Connection open";
	write(sock, message, strlen(message));

	while((read_size = recv(sock, client_message, 2000, 0)) > 0) {
		write(sock, client_message, strlen(client_message));
	}
	if (0 == read_size) {
		puts("Client disconnected");
		fflush(stdout);
	} else if (-1 == read_size) {
		perror("ERROR: recv failed");
	}

	free(socket_desc); 					/* free the socket pointer */
	return 0;
}