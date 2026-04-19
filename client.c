#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <openssl/evp.h>
#include <openssl/aes.h>

void rem_wrong(char* input) {
        char* g = input;
        while(*input) {
        if(*input >= 'a' && *input <= 'z') {
        *g = *input;
        g++;
                }
        input++;
        }
        *g = '\0';
}

void clear_arg(char* input) {
        char* g = input;
        while(*input) {
        if((*input >= 'a' && *input <= 'z') || (*input == '.') || (*input >= '0' && *input <='9')) {
        *g = *input;
        g++;
                }
        input++;
        }
        *g = '\0';
}

void parse_copy_args(char input[50] , int com_len , char args[2][15]) {
        int j = 0;
        while(input[com_len]!='-') {
        args[0][j] = input[com_len];
        com_len++;
        j++;
        if(j > 14)break;
        }

        args[0][j] = '\0';
        clear_arg(args[0]);

        int arg1_len = strlen(args[0]);
        memcpy(args[1] , input + com_len , 14);
        args[1][14] = '\0';
        clear_arg(args[1]);

}

void prepare_payload(char payload[100] , char command[5] , char args[2][15]);

int function_check(char* buffer) {

        if(buffer == NULL) {
        return 0;
        }

        if(strcmp(buffer,"echo") == 0) {
        return 22;
        }

        if(strcmp(buffer, "exit") == 0) {
        return 43;
        }

        if(strcmp(buffer, "creat") == 0) {
        return 23;
        }

        if(strcmp(buffer, "cp") == 0) {
        return 24;
        }

        if(strcmp(buffer , "rm") == 0) {
        return 25;
        }

        if(strcmp(buffer , "ls") == 0) {
        return 30;
        }

	if(strcmp(buffer , "put") == 0) {
	return 32;
	}

        return 0;
}


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
	ssize_t rr3 = recv(server_socket , auth , 4 , 0);
	if(rr3 <= 0) close(server_socket);
	if(rr3 > 0 && rr3 < 5) auth[rr3] = '\0';
	if(strstr(auth , "ACT\n") != 0) {
	ready = 1;
	}
	if(strstr(auth , "EXI\n") != 0) {
	close(server_socket);
	}





	if(ready == 1) {
	char input[50];
	char payload[100];
	char got[500];
	int function = 0;
	int dash_count = 0;
	char command[5];
	char args[2][15];

	ssize_t r = read(0 , input , 49);

	if(r > 0) input[r] = '\0';
	if(r <= 0) close(server_socket);

	memcpy(command , input , 4);
	command[4] = '\0';
	rem_wrong(command);

	function = function_check(command);

	if(function == 24) {

	for(int i = 0; i < r ; i++) {
	if(input[i] == '-')dash_count++;
	}

	if(dash_count <= 0 || dash_count > 1) {
	send(server_socket , "DED\n" , 4 , 0);
	close(server_socket);
	}

	if(dash_count == 1) {
	int com_len = strlen(command);
	parse_copy_args(input, com_len , args);
	}

	}

}


/*
	const char *buffer = input;

	send(server_socket , buffer , r , 0);

	int len = 499;
	if(function == 30) {
	ssize_t d = 0;
	while(( d = recv(server_socket , got , 499 , 0)) > 0) {

	if(strstr(got , "EOF\n") != 0) {
	break;
	}

	write(1 , got , d);

	}

}

}
*/
	close(server_socket);

}