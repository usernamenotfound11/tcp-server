#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


int main() {

	struct sockaddr_in sin;
	memset(&sin , 0 , sizeof(sin));

	sin.sin_port = htons(8080);
	sin.sin_family = AF_INET;
	inet_pton(AF_INET , "127.0.0.1" , &sin.sin_addr);

	int server_socket = socket(AF_INET , SOCK_STREAM , IPPROTO_TCP);

	if(connect(server_socket , (struct sockaddr *)&sin , sizeof(sin)) < 0) {
	perror("connect");
	close(server_socket);
	}

	char *input = calloc(50 , sizeof(char));
	char *got = calloc(500 , sizeof(char));
	ssize_t r = read(0 , input , 49);

	if(r > 0) input[r] = '\0';
	if(r <= 0) close(server_socket);

	const char *buffer = input;

	send(server_socket , buffer , r , 0);

	int len = 499;
	if(strstr(buffer , "ls") != 0) {
	ssize_t d = 0;
	while(( d = recv(server_socket , got , 499 , 0)) > 0) {

	if(strstr(got , "EOF\n") != 0) {
	break;
	}

	write(1 , got , d);

	}

}

	close(server_socket);

}
