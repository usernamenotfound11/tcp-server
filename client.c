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
	char message1[20];
	char message2[20];
	char username[16];
	char password[32];

	ssize_t rr1 = recv(server_socket , message1 , 19 , 0);
	if(rr1 <= 0) close(server_socket);
	if(rr1 > 0) message1[rr1] = '\0';
	if(write(1,message1,rr1) <= 0) {
	perror("write");
	close(server_socket);
	}

	ssize_t rr1q = read(0 , username ,15);
	if (rr1q <= 0) close(server_socket);
	if (rr1q > 0) username[rr1q] = '\0';
	send(server_socket , username , rr1q , 0);

        ssize_t rr2 = recv(server_socket , message2 , 19 , 0);
        if(rr2 <= 0) close(server_socket);
        if(rr2 > 0) message2[rr2] = '\0';
        if(write(1,message2,rr2) <= 0) {
        perror("write");
        close(server_socket);
        }

        ssize_t rr2q = read(0 , password ,32);
        if (rr2q <= 0) close(server_socket);
        if (rr2q > 0) password[rr2q] = '\0';
        send(server_socket , password , rr2q , 0);

	char auth[5];
	int ready = 0;
	recv(server_socket , auth , 4 , 0);
	if(strstr(auth , "ACT\n") != 0) {
	ready = 1;
	}

	if(ready == 1) {
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

}
	close(server_socket);

}