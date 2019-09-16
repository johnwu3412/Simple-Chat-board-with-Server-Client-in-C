struct message{
	char* msg;
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

struct thread* findThread(int ID);

struct user* findUser(int ID);

struct user* createUser();

struct thread* createThread();

int add(struct user *u, struct thread *thrd, char* message);

int removeMSG(int msgID);

void printThread(struct thread *thrd);

void printAll();

void printUser(struct user *u);

void sockprintThread(int sockfd, struct thread *thrd);

void sockprintAll(int sockfd);

void sockprintUser(int sockfd, struct user *u);
