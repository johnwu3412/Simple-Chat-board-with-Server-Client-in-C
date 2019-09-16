#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <stdio.h>
#include <sys/types.h> 
#include <pthread.h>
#include "chat_database_ops.h"
//#define PORT 9021


struct client_info{
	int id; //number associated with a client, will be matched with userID
	int socket;//file descriptor of client's socket
	char buf;
};

//global variables
pthread_mutex_t mtx;

//thread function
void * client_thrd(void *args){
	struct client_info client = *(struct client_info *)args;
	createUser();
	int sockfd = client.socket;
	printf("sockfd: %d\n", sockfd);
	int userID = client.id;
	client.buf = 1;
	int ret, i;
	char* readin;
	do{//runs indefinitely until client requests to exit
		printf("Reading...\n");
		if((ret = read(sockfd, &client.buf, 1))<=0){//if nothing exit
			printf("EXIT");
			break;
		}
		if(ret > 0){
			if(client.buf == 9){//if user requests exit, break and exit
				printf("%c",client.buf);
				printf("exit thread\n");
				pthread_exit(NULL);
				break;
			}else if(client.buf == 1){// user requests a user's history
				if((ret = read(sockfd, &client.buf, 1))<0){//read userID
					printf("EXIT");
					break;
				}
				printf("requesting user history\n");
				sockprintUser(sockfd, findUser((int)client.buf));
			}else if(client.buf == 2){//user requests a thread history
				if((ret = read(sockfd, &client.buf, 1))<0){//read threadID
					printf("EXIT");
					break;
				}
				printf("requesting thread history\n");
				sockprintThread(sockfd, findThread((int)client.buf));
			}else if(client.buf == 3){//requests all history
				printf("requesting all history\n");
				sockprintAll(sockfd);
			}else if(client.buf == 4){//requests createThread
				printf("creating thread\n");
				pthread_mutex_lock(&mtx);
				createThread();
				pthread_mutex_unlock(&mtx);
			}else if(client.buf == 5){//requests add to thread
				printf("adding to thread\n");
				char *str = (char *)malloc(100);
				if((ret = read(sockfd, &client.buf, 1))<0){//read threadID
					printf("EXIT");
					break;
				}
				int thrd = (int)client.buf;
				
				do{
					i = read(sockfd, &client.buf, 1);
					printf("buf: %c %d\n", client.buf, (int)client.buf );
					strcat(str, &client.buf);
				}while(client.buf != 0);
				add(findUser(userID), findThread(thrd), str);
			}
		}
	}while(1);
}

int main(int argc, char *argv[]){
	int sockfd, newsockfd1, clilen;
	char *buf;
	int pid;
	struct sockaddr_in serv_addr, cli_addr1, cli_addr2;
	const int PORT = atoi(argv[1]);
	createThread();
	pthread_t *thrds;
	pthread_t t;
	struct client_info* cli_arr;
	
	//creating socket
	printf("creating socket...\n");
	sockfd = socket(AF_INET, SOCK_STREAM,0);
	 
	if(sockfd < 0){
		perror("Can't open socket");
		exit(-1);
	}
	
	//initializing server address
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(PORT);	 
	
	//binding sockfd to the host address
	printf("binding socket...\n");
	if(bind(sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr))<0){
		perror("Can't bind");
		exit(-1);
	}
	
	//initializing mutex variable
	if(pthread_mutex_init(&mtx,NULL) != 0){
		perror("Mutex initialize error");
		exit(-1);
	}
	
	//halts program and waits for incoming connection
	printf("listening for connections...\n");
	listen(sockfd,5);
	clilen = sizeof(cli_addr1);
	
	
	//accepts two connections, fills in cli_addr properties
	int n;
	for(n = 1;;n++){
		printf("waiting for connection %d ...\n", n);
		newsockfd1 = accept(sockfd, (struct sockaddr*) &cli_addr1, &clilen);
		thrds = realloc(thrds, n*sizeof(pthread_t));
		cli_arr = realloc(cli_arr, n*sizeof(struct client_info));
		if(newsockfd1< 0){
			perror("Can't accept");
			exit(-1);
		}
		printf("connection %d received!\n", n);
		cli_arr[n-1].id = n-1;
		cli_arr[n-1].socket = newsockfd1;
		pthread_create(&t, NULL, client_thrd, (void*)(&cli_arr[n-1]));
	}
}