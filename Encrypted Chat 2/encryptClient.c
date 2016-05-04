/**
 *  encrypted chat client
 *  @syntax : ./encryptClient [ IP ADDRESS ] [ PORT NUMBER ]
 *  @note :   Compilation > gcc -std=c99 -pthread -c encryptClient.c -o encryptClient.o
 * 	      Object Link > gcc -std=c99 -pthread -o encryptClient encryptClient.o decrypt.o
 *  @author : Dandy Martin
 **/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <termios.h>
#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>

#define h_addr h_addr_list[0]
///Structs:
typedef struct{
    struct sockaddr_in addr;
    int connfd;
    int uid;
    char name[32];
}client_t;

///prototypes:
char *rem_header(char *msg);
char *decrypt(char *msg, unsigned int msg_len);
void read_message(void *client);
char *dup_msg(char *msg);

///ASM prototypes:
extern char *decryption_1(char *msg, unsigned int msg_len);
extern char *decryption_2(char *msg, unsigned int msg_len);
extern char *decryption_3(char *msg, unsigned int msg_len);
extern char *encryption(char *msg, unsigned int msg_len);

///preprocessed methods
void error(char *msg){
    perror(msg);
    exit(0);
}

///global variables
int exit_flag = 1;

/** @param : char *s - string to be stripped
 *  @def : This method removes carriage return and newline characters from
 *  passed string and appends null terminator to the end
 *  @return : void
 **/
void strip_newline(char *s){
	while(*s != '\0'){
		if(*s == '\r' || *s == '\n'){
			*s = '\0';
		}
		s++;
	}
}



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
    client_t *cli = (client_t *)malloc(sizeof(client_t));
    cli->connfd = sockfd;
    pthread_create(&threadID, NULL, (void *) &read_message, cli);

    while(exit_flag){
        bzero(buffer, 256);
        fgets(buffer, 255, stdin);
        int msg_size = strlen(buffer);
        char *temp = dup_msg(buffer);
        encryption(buffer, msg_size);
        strip_newline(buffer);

        if(strncmp("_exit!", temp, 6) != 0){
            if((n = write(sockfd, buffer, strlen(buffer))) < 0){
                error("ERROR writing to socket");
            }
        }else{
            exit_flag = 0;
            printf("CHAT EXITED\n");
        }
        bzero(buffer, 256);
        free(temp);
    }



    exit(EXIT_SUCCESS);
    return 0;

}

/**
 *  @param : int nsfd - socket file descriptor
 *  @def : This method retrieves the message received from the socket, and prints
 *  to STDOUT after decryption and header stripping.
 *  @return : void
 **/
void read_message(void *client){
    client_t *cli = (client_t *) client;
    int nsfd = cli->connfd;
    while(exit_flag){
        char buffer[256];
        int n;
        bzero(buffer, 256);
        if((n = read(nsfd, buffer, 255)) < 0){
            error("ERROR reading from socket");
        }else{
            int size = strnlen(buffer, 255);
            char *temp = decrypt(buffer, size);
            temp = rem_header(temp);
            strip_newline(temp);
            printf("%s\n", temp);
        }
        bzero(buffer, 256);
    }
}

/**
 *  @param : char *msg - received message
 *  @def : This method duplicates and returns the string value provided. It also dynamically sizes
 *  the returned string by appending the null terminating character to the end.
 *  This message was written since strcat/strncat do not function the way the program needs.
 *  @return : pointer toward duplicated string is returned upon completion
 *  @note   : >>>MEMORY LEAK<<< malloc() called with no associated free() - errors encountered when called
 **/
char *dup_msg(char *msg){
    char *temp = malloc(strlen(msg) + 1);
    int count = strlen(msg);
    for(int i = 0; i < count; i++){
        temp[i] = msg[i];
        temp[i + 1] = '\0';
    }
    return temp;
}

/**
 *  @param : char *msg - received message, unsigned int msg_len - length of msg string
 *  @def : This method is designed to perform the three different encryption
 *  methods on the received string. The header/key of these messages is then compared
 *  to the correct header.
 *  @return : The message is returned upon correct decryption strategy found.
 *  Default return is the third strategy as it is the default in the server encryption
 *  switch statement.
 **/
char *decrypt(char *msg, unsigned int msg_len){
    char *header = "HZQ";
    int header_size = strlen(header);
    int count = 0;
    char *temp[3] = {decryption_1(dup_msg(msg), msg_len),
    decryption_2(dup_msg(msg), msg_len), decryption_3(dup_msg(msg), msg_len)};
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < header_size; j++){
            if(header[j] == temp[i][j]){
                count++;
            }
        }
        if(count == 3){
            return temp[i];
        }
        count  = 0;
    }
    return temp[2];
}

/**
 *  @param : char *msg - received message
 *  @def : This method shifts the values in the array toward 0 by 3 positions, and appends the
 *  null terminating character to the end.
 *  @return : returns corrected message without header/key
 **/
char *rem_header(char *msg){
    int msg_len = strlen(msg);;
    for(int i = 0; i < msg_len; i++){
        if(i >= 3){
            msg[i - 3] = msg[i];
            msg[i - 2] = '\0';
        }
    }
    return msg;
}
