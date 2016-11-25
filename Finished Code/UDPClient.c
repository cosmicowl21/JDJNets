#include <errno.h>
 #include <sys/types.h>
 #include <netdb.h>
 #include <netinet/in.h>
 #include <stdlib.h>
 #include <string.h>
 #include <sys/socket.h>
 #include <stdio.h>
 #include <syslog.h>
 #include <signal.h>
 #include <unistd.h>

#define PORT 1066
#define MAXLEN 1024

int main()
{
	char message[MAXLEN];
	int c_socket, usernameCheck = 0;
	struct sockaddr_in c_sock_address_info, server_address_info;
	c_socket = socket(AF_INET, SOCK_DGRAM, 0);

	c_sock_address_info.sin_family = AF_INET;
 	c_sock_address_info.sin_addr.s_addr = htonl(INADDR_ANY);
 	c_sock_address_info.sin_port = htons(PORT);
	while(usernameCheck == 0)
	{
		printf("Username:\n(Username must be 12 characters)\n");
		scanf("%s", message);
		if(strlen(message) == 12)
		{
			usernameCheck = 1;
			message[strlen(message)] = '\0';
		}
		else
		{
			printf("Username not long enough\n");
		}
	}
	server_address_info.sin_family = AF_INET;
 	server_address_info.sin_addr.s_addr = htonl(INADDR_ANY);
 	server_address_info.sin_port = htons(PORT);

	sendto(c_socket, message, strlen(message), 0, &server_address_info, sizeof server_address_info);
	recvfrom(c_socket, message, MAXLEN, 0, NULL, NULL);
	printf("%s", message);

	




	close(c_socket);
}
