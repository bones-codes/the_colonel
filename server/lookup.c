#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>


int main(void) {
	char *hostname = "www.google.com";
	char ip[100];
	struct hostent *he;
	struct in_addr **addr_list;
	int i;

	if (NULL == (he = gethostbyname(hostname))) {
		herror("gethostbyname -- ERROR");
		return 1;
	}
	
	addr_list = (struct in_addr **) he->h_addr_list;	/* Cast the h_addr_list to in_addr , 
	 													 * since h_addr_list has the ip in long format */

	for (i = 0; addr_list[i] != NULL; i++) {
		strcpy(ip, inet_ntoa(*addr_list[i]));			/* return the first one */
	}

	printf("%s resolved to: %s\n", hostname, ip);
	return 0;
}