//client.c Client demo for nweb, 'A Tiny, Safe Webserver' by Nigel Griffiths
//	-compiles on glue with: "$ gcc -Wall  client.c -lsocket -lresolv -lnsl -o client " 
//	-runs on glue with  "$ ./client
//!!note: PORT and IP_ADDRESS below must match those which nweb was started with

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>

//hardcoded server IP (too lazy to do it better) 
//#define PORT        8321			/* port number is an int */
//#define IP_ADDRESS "128.8.70.200"	/* IP address is a string, discovered with '(glue)% ifconfig -a' */

#define BUFSIZE 1

int pexit(char * msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char* argv[])
{
	
	const int PORT = atoi(argv[2]);
	const char* IP_ADDRESS = argv[1];
	int sockfd;
	char buf = -1;
	char readin[BUFSIZE];
	int ret;
	int i;
	static struct sockaddr_in serv_addr;
	
	
	printf("client trying to connect to %s and port %d\n",IP_ADDRESS,PORT);
	if((sockfd = socket(AF_INET, SOCK_STREAM,0)) <0) 
		pexit("socket() failed");

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS);
	serv_addr.sin_port = htons(PORT);

	if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) <0)
		pexit("connect() failed");

	/* now the sockfd can be used to communicate to the server */
	//sends to server the get command
	
	while(1){
		printf("\nEnter command: \n");
		buf = getchar();
		getchar();
		buf = atoi(&buf);
		write(sockfd, &buf, 1);
		if(buf == 9){ //exit 
			printf("EXIT\n");
			break;
		}else if(buf == 1){// printing user history
			printf("Enter User ID: \n");
			buf = getchar();
			getchar();
			buf = atoi(&buf);
			write(sockfd, &buf, 1);
			do{
				i=read(sockfd,readin,BUFSIZE);
				write(1,readin,i);
			}while(*readin != 0);
		}else if(buf == 2){// printing thread history
			printf("Enter Thread ID: \n");
			buf = getchar();
			getchar();
			buf = atoi(&buf);
			write(sockfd, &buf, 1);
			do{
				i=read(sockfd,readin,BUFSIZE);
				write(1,readin,i);
			}while(*readin != 0);
		}else if(buf == 3){//printing all 
			do{
				i=read(sockfd,readin,BUFSIZE);
				write(1,readin,i);
			}while(*readin != 0);
		}else if(buf == 4){//create thread
			printf("new thread created!\n");
		}else if(buf == 5){//add 
			printf("Enter Thread ID: \n");
			buf = getchar();
			getchar();
			buf = atoi(&buf);
			write(sockfd, &buf, 1);
			
			printf("Enter message: \n");
			char message[100];
			char x = 0;
			gets(message);
			write(sockfd, message, strlen(message));
			write(sockfd, &x, 1);
			printf("added\n");
			
		}
	}
	
	return 1;
}
