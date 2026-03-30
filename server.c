#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <poll.h>
#include <dirent.h>

void write_log(); //wip

char* function(char** p) {

        if(!*p) return NULL;


        char *s = *p;

        char *c = strchr(s , ' ');
        char *d = strchr(s , '\n');

    if(!c && !d) {
        return NULL;
    }

        char* text = NULL;
        if(d && (!c || d < c )) {
        if(d)*d = '\0';
        text = s;
        *p = d + 1;
        return text;
        }
        *c = '\0';
        text = s;
        *p = c + 1;

        return text;
}


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

	if(strcmp(buffer, "create") == 0) {
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

	return 0;
}

struct arg{
	char *text;
};

struct user{
	char *username;
	unsigned char *password;
	int fd;
	int id;
};		//wip

int main() {

	struct sockaddr_in sin;

	memset(&sin , 0 , sizeof(sin));

	sin.sin_port = htons(8080);
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;

	int server_socket = socket(AF_INET , SOCK_STREAM , IPPROTO_TCP);
	int opt = 1;
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	int binded = bind(server_socket , (struct sockaddr *)&sin , sizeof(sin));

	listen(server_socket , 2);
	struct pollfd fds[10];

	int nfds = 1;
	fds[0].fd = server_socket;
	fds[0].events = POLLIN;

	while(1) {

	int ret = poll(fds , nfds , -1);

	if(ret == -1) {
	perror("poll");
	break;
		}

	for(int i=0; i<nfds; i++) {
	if(fds[i].fd == server_socket) {
	if(fds[i].revents & POLLIN) {
 
	int client = accept(server_socket , 0 , 0);
	if(client > 0) {
	fds[nfds].fd = client;
	fds[nfds].events = POLLIN;
	nfds++;
		}


		}
	}else {
	struct arg args[5];
	for(int i=0; i<5;i++) {
	args[i].text = NULL;
	}
        char *buffer = calloc(100 , sizeof(char));
        int len = 100;
        ssize_t r = recv(fds[i].fd , buffer , len - 1, 0);

	if (r == 0) {
    	close(fds[i].fd);
    	fds[i] = fds[nfds - 1];
	    nfds--;
    	i--;
    	free(buffer);
	    continue;
}

	if(r < 0) {
	perror("recv");
	free(buffer);
	continue;
	}


        if(r > 0) {
        char *p = buffer;
	char *result = function(&p);
	args[0].text = result;
	int checker = function_check(args[0].text);


	//exit
	if(checker == 43) {
	printf("disconnect[!]\n");
        close(fds[i].fd);
        fds[i] = fds[nfds - 1];
            nfds--;
        i--;
        free(buffer);
            continue;
	}


	//echo
	if(checker == 22) {
	printf("echo[!]\n");
	for(int i=1;i<2;i++) {
	char *temp = function(&p);
	if(!temp) {
        break;
	}
	args[i].text = temp;
	printf("%s\n" , args[i].text);
		}

	}


	//create
	if(checker == 23) {
	printf("create[!]\n");
	for(int i=1; i<3;i++) {
	char *temp = function(&p);
	if(!temp) {
	break;
		}
	args[i].text = temp;
	}

	if(args[2].text == NULL) {
	args[2].text = "0644";
	}
	mode_t mode = strtol(args[2].text, NULL, 8);
	int fd = open(args[1].text , O_CREAT | O_TRUNC | O_RDWR , mode);
	close(fd);
	}


	//copy
	if(checker == 24) {
	char* copied = calloc(4096 , sizeof(char));
        for(int i=1; i<3;i++) {
        char *temp = function(&p);
        if(!temp) {
        break;
                }
        args[i].text = temp;
        }

	int fd = open(args[1].text , O_RDONLY , 0644);
	int fd1 = open(args[2].text , O_WRONLY , 0644);

	ssize_t abc = read(fd , copied , 4096);
	write(fd1 , copied , abc);
	close(fd);
	close(fd1);
	}


	//remove[wip]
	if(checker == 25) {
	printf("remove[!]\n");
	}


	if(checker == 30) {
	printf("ls[!]\n");
	DIR* dir = opendir(".");
	if(!dir) {
	break;
		}
	struct dirent *entry;

	while((entry = readdir(dir)) != NULL) {
	if(strcmp(entry->d_name , ".") == 0 || strcmp(entry->d_name , "..") == 0) {
	continue;
		}
	send(fds[i].fd , entry->d_name , strlen(entry->d_name) , 0);
	send(fds[i].fd , "\n" , 1 , 0);
	}
	send(fds[i].fd , "EOF\n" , 4 , 0);
	closedir(dir);

	}


			}
		}

	}

	}

	close(server_socket);
}

