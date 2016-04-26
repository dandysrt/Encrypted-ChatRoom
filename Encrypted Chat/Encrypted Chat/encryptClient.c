/**
 *  encrypted chat client
 *  @syntax : ./encryptClient [ IP ADDRESS ] [ PORT NUMBER ]
 *  @note   :
 *  @author : Dandy Martin
 **/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <termios.h>
#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>

#define h_addr h_addr_list[0]

///ASM prototypes:
extern char *decryption_1(char *msg, unsigned int msg_len);
extern char *encryption(char * msg, unsigned int msg_len);

///preprocessed methods
void error(char *msg){
    perror(msg);
    exit(0);
}

void strip_newline(char *s){
	while(*s != '\0'){
		if(*s == '\r' || *s == '\n'){
			*s = '\0';
		}
		s++;
	}
}

void read_message(int nsfd);

int main(int argc, char *argv[]){

    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    pthread_t threadID;
    struct hostent *server;

    char buffer[256];

    if(argc < 3){
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }

    portno = atoi(argv[2]);

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        error("ERROR opening socket");
    }

    if((server = gethostbyname(argv[1])) == NULL){
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;

    bcopy((char *)server-> h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);

    serv_addr.sin_port = htons(portno);

    if(connect(sockfd, &serv_addr, sizeof(serv_addr)) < 0){
        error("ERROR connecting");
    }else{
        printf("Connected.\n");
    }

    pthread_create(&threadID, NULL, (void *) &read_message, sockfd);
    while(1){
        bzero(buffer, 256);
        fgets(buffer, 255, stdin);
        int msg_size = strlen(buffer);
        encryption(buffer, msg_size);
        strip_newline(buffer);
        if((n = write(sockfd, buffer, strlen(buffer))) < 0){
                error("ERROR writing to socket");
        }
        bzero(buffer, 256);
    }

    return 0;

}

void read_message(int nsfd){
    while(1){
        char buffer[256];
        int n;
        bzero(buffer, 256);
        if((n = read(nsfd, buffer, 255)) < 0){
            error("ERROR reading from socket");
        }else{
            int size = strlen(buffer);
            char *temp = decryption_1(buffer, size);
            strip_newline(temp);
            printf("%s\n", temp);
        }
        bzero(buffer, 256);
    }
}


