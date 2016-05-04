/**
 *  encrypted chat server
 *  @syntax : ./encryptServer [ PORT NUMBER ]
 *  @note   : Compilation > gcc -std=c99 -pthread -c encryptServer.c -o encryptServer.o
 * 	      Object Link > gcc -std=c99 -pthread -o encryptServer encryptServer.o encrypt.o
 *  @author : Dandy Martin
 **/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <strings.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>

#define MAX_CLIENTS 10
#define ERROR_MARGIN 3
#define MAX_MESSAGES 5
#define HEADER "hello"


///Structs:
typedef struct{
    struct sockaddr_in addr;
    pthread_t c_threadID;
    int connfd;
    int uid;
    char name[32];
}client_t;

///Prototypes:
void handle_client(void *client);
void handle_send(void *client);
char *dequeue();
char *encrypt(char *msg);
void queue(char *msg);
void queue_add(client_t *cl);
void queue_delete(int uid);
char *add_header(char *msg);

///ASM Prototypes:
extern char *encryption_1(char *msg, unsigned int msg_len);
extern char *encryption_2(char *msg, unsigned int msg_len);
extern char *encryption_3(char *msg, unsigned int msg_len);

///Global Variables:
char *message_queue[MAX_MESSAGES];
client_t *clients[MAX_CLIENTS];
static unsigned int msg_count = 0, client_count = 0, msg_ptr = 0;
static int uid = 10;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

///preprocessed methods
void error(char *msg){
    perror(msg);
    exit(1);
}

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

    unsigned int sockfd, newsockfd, portno, clilen;
    //pthread_t threadID;                                             //thread id
    struct sockaddr_in serv_addr, cli_addr;


    if(argc < 2){
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){

        error("ERROR opening socket");

    }

    bzero((char *) &serv_addr, sizeof(serv_addr));

    portno = atoi(argv[1]);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if(bind(sockfd, (struct sockaddr * ) & serv_addr, sizeof(serv_addr)) < 0){
        error("ERROR on binding");
    }

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    ///Create new thread to run handle_client() upon client acceptance
    while(1){

        if((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) < 0){
            error("ERROR on accept");
        }else{
            pthread_t threadID;
            client_t *cli = (client_t *)malloc(sizeof(client_t));
            cli->c_threadID = threadID;
            cli->addr = cli_addr;
            cli->connfd = newsockfd;
            cli->uid = uid++;
            sprintf(cli->name, "%d", cli->uid);
            queue_add(cli);
        }
    }

   return 0;

}

/**
 *  @param : nsfd - socket file descriptor
 *  @def : Exchange information with client and maintain message_queue[] array
 *  @return : void
 **/
void handle_client(void *client){
    client_t *cli = (client_t *) client;
    int nsfd = cli->connfd;
    char buffer[256];//--------------------------------buffer for sending-receiving messages
    int n;
    bzero(buffer, 256);
    while(1){

        if((n = read(nsfd, buffer, 255)) < 0){
            error("ERROR reading from socket");
        }
        printf("read: %s\n", buffer);
        strip_newline(buffer);
        strcpy(buffer,add_header(buffer));
        queue(buffer);
        handle_send(client);
        bzero(buffer, 256);
    }

}

/**
 *  @param : void *client - client struct
 *  @def : send information to other clients in client queue
 *  @return : void
 **/
void handle_send(void *client){
    client_t *cli = (client_t *) client;
    char buffer[256];
    int n;
    bzero(buffer, 256);
    if(msg_count > 0){
        printf("DEBUG: send\n");
        char *temp = dequeue();
        temp = encrypt(temp);//-------------------------encrypt msg from queue and add to buffer
        strcat(buffer, temp);//-------------------------before sending
        printf("DEBUG TEMP: %s\n", temp);
        printf("sending: %s\n", buffer);
        strip_newline(buffer);
        for(int i = 0; i < client_count; i++){
            if(cli->uid != clients[i]->uid){
                if((n = write(clients[i]->connfd, buffer, 255)) < 0){
                    error("ERROR writing to socket");
                }
            }
        }
        msg_count--;
        msg_ptr--;
        bzero(buffer, 256);
    }

}

/**
 *  @param : received string
 *  @def : This method places received message string into queue
 *  to be retreived elsewhere.
 *  @return : void
 **/
void queue(char *msg){
    printf("DEBUG MSG: %s\n", msg);
    if(msg_count < MAX_MESSAGES){
        pthread_mutex_lock(&mtx);
            message_queue[msg_count] = msg;
            msg_count++;
        pthread_mutex_unlock(&mtx);

        printf("DEBUG QUEUED MSG: %s\n", message_queue[msg_count - 1]);
        printf("DEBUG msg_count: %d\n", msg_count);
    }else{
        pthread_mutex_lock(&mtx);
            message_queue[0] = msg;
            msg_count = 1;
            msg_ptr = 0;
        pthread_mutex_unlock(&mtx);
    }

}

/**
 *  @param : none
 *  @def : This method retrieves next message from the message queue
 *  @return : message string
 **/
char *dequeue(){
    printf("DEBUG: dequeue\n");

    char *temp = "\0";
        pthread_mutex_lock(&mtx);
            if(msg_count > 0){
            printf("DEBUG MSG_COUNT: %d\n", msg_count);
                temp = message_queue[msg_ptr];
                msg_ptr++;
                printf("idek anymore: %s\n", temp);
            }
            printf("DEBUG DEQUEUE MSG: %s\n", message_queue[msg_ptr - 1]);
        pthread_mutex_unlock(&mtx);
    return temp;
}

/**
 *  @param : client_t *cli - generated client struct
 *  @def : This method adds a client to the server's overall list of clients
 *  @return : void
 **/
void queue_add(client_t *cli){
    pthread_create(&cli->c_threadID, NULL, (void *) &handle_client, cli);
    pthread_mutex_lock(&mtx);
        if(client_count < MAX_CLIENTS){
            clients[client_count] = cli;
            client_count++;
        }
    pthread_mutex_unlock(&mtx);
}

/**
 *  @param : char *msg - string to be encrypted
 *  @def : This method randomly selects an encryption method for passed string
 *  @return : encrypted string
 **/
char *encrypt(char *msg){
    srand(time(NULL));
    unsigned int choice = ((rand() % 30) / 10) + 1;
    unsigned int size = strlen(msg);
    printf("SIZE OF MSG: %d\n", size);
    char* temp = "\0";
    printf("RANDOM SELECTION: %d\n", choice);
    switch(choice){
        case 1:
            temp = encryption_1(msg, size);
        break;
        case 2:
            temp = encryption_2(msg, size);                        ///these choose the server encryption method
        break;
        case 3: //fall-through case
        default:
            temp = encryption_3(msg, size);
        break;
    }
    printf("encrypted: %s\n", msg);

    return temp;
}

/**
 *  @param : char *msg - string to be appended
 *  @def : This method appends a header/key to the front of a string
 *  prior to secondary encryption
 *  @return : appended string
 **/
char *add_header(char *msg){
    char *header = malloc(strlen(msg) + 5);
    strcpy(header, "HZQ");
    printf("DEBUG ADD_HDR\n");
    return strcat(header, msg);
}
