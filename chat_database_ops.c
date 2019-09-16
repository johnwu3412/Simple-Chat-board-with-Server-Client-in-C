#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct message{
	char* msg;
	int messageID;
	int userID;
	int threadID;
	struct message *prevThrd;
	struct message *nextThrd;
	struct message *prevUser;
	struct message *nextUser;
	struct message *prevAll;
	struct message *nextAll;
};
struct thread{
	int threadID;
	struct message *head;
	struct message *last;
};
struct user{
	int userID;
	struct message *head;
	struct message *last;
};
static int messageIDcount = 0;
static int userIDcount = 0;
static int threadIDcount = 0;
static struct message *firstAll = NULL;
static struct message *lastAll = NULL;
static struct user *users = NULL;
static struct thread *threads = NULL;

//if ID is found, return the thread or null otherwise
struct thread* findThread(int ID){
	if(ID > threadIDcount-1){
		return NULL;
	}else{
		return &threads[ID];
	}
}

//if ID is found, return the user or null otherwise
struct user* findUser(int ID){
	if(ID > userIDcount-1){
		return NULL;
	}else{
		return &users[ID];
	}
}

//creates a user with userID based on the number of userIDs, need to append to list of users
struct user* createUser(){
	struct user *new;
	new = (struct user *) malloc(sizeof(struct user));
	new->userID = userIDcount;
	new->head = NULL;
	new->last = NULL;
	userIDcount++;
	
	if(users == NULL){
		users = new;
	}else{
		users = realloc(users, sizeof(struct user)*(userIDcount));
		users[userIDcount-1] = (*new);
	}
	return new;
}

//creates a thread with threadID based on number of existing threads, need to append to list of threads
struct thread* createThread(){
	struct thread *new = (struct thread *)malloc(sizeof(struct thread));
	new->threadID = threadIDcount;
	new->head = NULL;
	new->last = NULL;
	threadIDcount++;
	
	
	threads = realloc(threads, sizeof(struct thread)*(threadIDcount));
	threads[threadIDcount-1] = (*new);
	
	return new;
}

//adds message to a user, a thread, and the global message pool
int add(struct user *u, struct thread *thrd, char *msg){
	
	//creating a new message 
	struct message *new;
	new = (struct message *)malloc(sizeof(struct message));
	new->msg = msg;
	new->prevThrd = thrd->last;
	new->prevUser = u->last;
	new->prevAll = lastAll;
	new->messageID = messageIDcount++;
	new->userID = u->userID;
	new->threadID = thrd->threadID;
	
	if(thrd->head == NULL){
		thrd->head = new;
		thrd->last = new;
	}else{
		thrd->last->nextThrd = new;
		thrd->last = new;
	}
	
	if(u->head == NULL){
		u->head = new;
		u->last = new;
	}else{
		u->last->nextUser = new;
		u->last = new;
	}
	
	if(firstAll == NULL){
		firstAll = new;
		lastAll = new;
	}else{
		lastAll->nextAll = new;
		lastAll = new;
	}
	
	return 1;
}

//if ID is present remove the message
int removeMSG(int msgID){
	struct message *temp = firstAll;
	
	//finds the message
	while(temp != NULL && temp->messageID != msgID){
		temp = temp->nextAll;
	}
	
	//if the message was not found
	if(temp == NULL){
		return -1;
	}
	
	//remove from all list
	
	//case -- first in the list
	if(temp == firstAll){
		firstAll = temp->nextAll;
	}else{
		temp->prevAll->nextAll = temp->nextAll;
	}
	
	//case -- last in the list
	if(temp == lastAll){
		lastAll = temp->prevAll;
	}else{
		temp->nextAll->prevAll = temp->prevAll;
	}
	
	//remove from user list
	struct user* usr = findUser(temp->userID);
	if(temp == usr->head){
		usr->head = temp->nextUser;
	}else{
		temp->prevUser->nextUser = temp->nextUser;
	}
	
	if(temp == usr->last){
		usr->last = temp->prevUser;
	}else{
		temp->nextUser->prevUser = temp->prevUser;
	}
	
	//remove from thread list
	struct thread* thrd = findThread(temp->threadID);
	if(temp == thrd->head){
		thrd->head = temp->nextThrd;
	}else{
		temp->prevThrd->nextThrd = temp->nextThrd;
	}
	
	if(temp == thrd->last){
		thrd->last = temp->prevThrd;
	}else{
		temp->nextThrd->prevThrd = temp->prevThrd;
	}
	
	return 0;
	
	
}

//prints all messages inside of a thread
void printThread(struct thread *thrd){
	if(thrd == NULL){
		printf("NULL THREAD");
		return;
	}
	struct message *mes = thrd->head;
	printf("THREAD ID: %d MESSAGES------------------------\n",thrd->threadID);
	while(mes != NULL){
		printf("%d %d %d %s\n", mes->messageID, mes->userID, mes->threadID, mes->msg);
		mes = mes->nextThrd;
	}
	printf("END OF THREAD %d MESSAGES --------------------------\n", thrd->threadID);
}

//prints all messages sent into the program
void printAll(){
	struct message *mes = firstAll;
	printf("HISTORY -------------------\n");
	while(mes != NULL){
		printf("%d %d %d %s\n", mes->messageID, mes->userID, mes->threadID, mes->msg);
		mes = mes->nextAll;
	}
	printf("END OF ALL MESSAGES-------------------\n");
}

//prints all messages sent by a user
void printUser(struct user *u){
	if(u == NULL){
		printf("NULL USER");
		return;
	}
	struct message *mes = u->head;
	printf("USER ID: %d MESSAGES-------------------\n", u->userID);
	
	while(mes != NULL){
		printf("%d %d %d %s\n", mes->messageID, mes->userID, mes->threadID, mes->msg);
		mes = mes->nextUser;
	}
	printf("END OF USER %d MESSAGES -----------------------\n", u->userID);
}

void sockprintThread(int sockfd,struct thread *thrd){
	char *buffer;
	char x = 0;
	
	if(thrd == NULL){
		printf("NULL THREAD");
		write(sockfd, &x, 1);
		return;
	}
	struct message *mes = thrd->head;	
	printf("Writing thread history...\n");
	sprintf(buffer, "Thread ID: %d MESSAGES-------------------\n", thrd->threadID);
	if(write(sockfd, buffer, strlen(buffer)) < 0){
		printf("ERROR\n");
	}
	while(mes != NULL){
		sprintf(buffer, "%d %d %d %s\n", mes->messageID, mes->userID, mes->threadID, mes->msg);
		write(sockfd, buffer, strlen(buffer));
		mes = mes->nextThrd;
	}
	sprintf(buffer, "END OF THREAD %d MESSAGES -----------------------\n", thrd->threadID);
	write(sockfd, buffer, strlen(buffer));
	write(sockfd, &x, 1); 
}

void sockprintAll(int sockfd){
	struct message *mes = firstAll;
	char* buffer;
	sprintf(buffer,"HISTORY -------------------\n");
	write(sockfd, buffer, strlen(buffer));
	while(mes != NULL){
		sprintf(buffer, "%d %d %d %s\n", mes->messageID, mes->userID, mes->threadID, mes->msg);
		write(sockfd, buffer, strlen(buffer));
		mes = mes->nextAll;
	}
	sprintf(buffer, "END OF ALL MESSAGES-------------------\n");
	write(sockfd, buffer, strlen(buffer));
	char x = 0;
	write(sockfd, &x, 1); 
}

void sockprintUser(int sockfd, struct user *u){
	char *buffer;
	
	char x = 0;
	if(u == NULL){
		printf("NULL USER");
		write(sockfd, &x, 1);
		return;
	}
	struct message *mes = u->head;	
	printf("Writing user history...\n");
	sprintf(buffer, "USER ID: %d MESSAGES-------------------\n", u->userID);
	if(write(sockfd, buffer, strlen(buffer)) < 0){
		printf("ERROR\n");
	}
	while(mes != NULL){
		sprintf(buffer, "%d %d %d %s\n", mes->messageID, mes->userID, mes->threadID, mes->msg);
		write(sockfd, buffer, strlen(buffer));
		mes = mes->nextUser;
	}
	sprintf(buffer, "END OF USER %d MESSAGES -----------------------\n", u->userID);
	write(sockfd, buffer, strlen(buffer));
	write(sockfd, &x, 1); 
}