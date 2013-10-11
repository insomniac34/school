/* 
 * programmer: Tyler Raborn
 * TESTING ENVIRONMENTS: Linux Mint v14 NADIA 64-bit running in Oracle VirtualBox on Windows 8 Pro 64-bit, THOT@cs.pitt.edu
 * COMPILATION COMMAND UTILIZED: gcc -Wall -o client client.c
 */ 

//library calls
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>

//preprocessor commands
#define true 1;
#define false 0;

//function prototypes
void* clientInstance(void*);
void prompt();
void info();
void error(const char*);

//global variables
int cnt=0;
int eof = false; 
FILE* outFile;

//global thread objects
pthread_mutex_t mutex_0 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;
pthread_t *threadList; //dynamic array of threads
typedef struct //server object; holds ip address and port
{
    int port;
    char* ip;
} Server;

int main(int argc, char ** argv)
{
	Server *serverList; //dynamic array of server structs!@
    //io stuffs
	const char* ipstring = "255.255.255.255";
	int socket_interval = 16 * (argc/2);
	int ip_interval = sizeof(ipstring)*(argc/2);
	char** socketName = (char**)malloc(sizeof(socket_interval)*(argc/2)); //at most 4 
	char** ipaddr = (char**)malloc(sizeof(ip_interval)*(argc/2)); //allocate argc/2 * 32 bit ip's
	
	/******************************user i/o*******************************/
    /*Valid arguments: ./program <arg1> <arg2> <...> <...> <argn> <argn+1>*/
    int numThreads=0;
	numThreads=((argc-1)/2);
	serverList = (Server*)malloc(numThreads * sizeof(Server));

	if ((argc-1)%2==1)
		error("\nInvalid number of arguments; accepted input: <ipaddr1> <port1> <ipaddr2> <port2> ... ");
	long int * ipArray[argc/2]; //variable assignment loop
	int * sockArray[argc/2];	
	int counter = 1;
	int i;
	for (i=1;i<argc;i+=2,cnt++)
	{
        //this block takes in the arguments for later usage. It's pretty ugly, but in a beautiful 80's C kind of way IMO
		char * ipptr = *((argv)+i); //value pointed to by socket + # of bits counter * 8...
		*(ipaddr + (counter * 32)) = ipptr;
		ipArray[counter-1] = (long int*)*((argv)+i);
		printf("%s\n", (char*)ipArray[counter-1]);
		char * sockptr = *((argv)+i+1);
		*(socketName + (counter * 16)) = sockptr; 
		sockArray[counter-1] = (int*)*((argv)+i+1);
		printf("%s\n", (char*)sockArray[counter-1]);
        serverList[cnt].port=atoi(argv[i+1]); //assigns serverList its values from user input
		serverList[cnt].ip=argv[i];
    	
		counter++;
	}
	cnt=0;
    printf("\n                      Welcome to \n\n\n\n\n\n\n\n                      <=THE TYRATE BAY=>\n                         v0.1\n                      \n                               \n\n\n\n\n\n\n\n\n - The world's #1 p2p filesharing client designed to get you what you want, when you need it! \nTHIS SOFTWARE IS DISTRIBUTED WITH THE GNU FREE PUBLIC LICENSE ");
    prompt(); //menu
    //open/close....createfile
    outFile=fopen("output.txt", "w"); 
    fclose(outFile);
    //LEARNED THE HARD WAY always typecast to ptr type when using MALLOC!!!
    threadList=(pthread_t*)malloc(numThreads * sizeof(pthread_t)); 
    //holds pthreadCreate val
    int newThread = 0;
    //creates <numThreads> individually-threaded clients
    for (i=0;i<numThreads;i++) //for(i=# of servers...)...
    {
    	Server* servptr=&serverList[i]; //get addr of serverList at location i
    	newThread=pthread_create(&threadList[i], NULL, &clientInstance, (void*)servptr); //create clientInstance...instances...
        if(newThread!=0)
            error("pthread_create() error");
    }

    //***************************JOIN() THREADS HERE**********************************/
    for(i=0; i<numThreads;i++)
    	pthread_join(threadList[i], NULL); 

    //*******************************DESTROY() THREADS********************************/
    //kill 'em all
    pthread_mutex_destroy(&mutex_0);
    pthread_mutex_destroy(&mutex_1);
    printf("\nThanks for choosing THE TYRATE BAY. Have a safe, and productive, day! "); //thx
	return 0;
}

/*  "function pointers are pretty much C's ghetto version of polymorphism"
	- Professor Hoffman, Spring 2013 void* lecture. 

                        N E V E R   F O R G E T
*/
void* clientInstance(void* arg) //a pseudo-polymorphic client function
{
	int sock_fd; //socket file descriptor
	Server* localServer = (Server*)arg; //utilize void* param
	char buffer[1025]; //1024+1, gotta make room for 
	struct sockaddr_in addr;
    int idx;

    //initialize addr struct
    memset(addr.sin_zero, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(localServer->port);
    addr.sin_addr.s_addr = inet_addr(localServer->ip);

    /*************************receive file pieces***********************/
    int seekptr;
    while(!eof) 
    {
    	///////////////////lock()////////////////////////
        pthread_mutex_lock(&mutex_0);
        idx=cnt++; //INDEX VARIABLES; count # of char[1025]'s needed

        ///////////////////unlock()///////////////////////
        pthread_mutex_unlock(&mutex_0);
        printf("retrieving package %d\n", idx);

        ///////////////////socket()///////////////////////
        sock_fd = socket(AF_INET, SOCK_STREAM, 0);
        if(sock_fd<0) 
            error("socket() error: ");

        ///////////////////connect()///////////////////////
        if(connect(sock_fd, (struct sockaddr*)&addr, sizeof(addr))<0)
            error("connect() error: ");
        memset(buffer, 0, sizeof(buffer)); //memset

        //PRINTF TO BUFFER...SO SPRINTF
        sprintf(buffer, "%d", idx);
        write(sock_fd, buffer, strlen(buffer)); 
        memset(buffer, 0, sizeof(buffer)); //memset agian
        read(sock_fd, buffer, 1025);

        ///////////////////lock()////////////////////////
        pthread_mutex_lock(&mutex_1);
        outFile = fopen("output.txt", "r+");

        //find prev location via idx var and bufsize
        seekptr=fseek(outFile, 1024 * idx, SEEK_SET); //fseek()...i HATEEE fseek()
        if(seekptr!=0)
        	error("fseek() error: ");

        fwrite(buffer, 1, strlen(buffer), outFile);
        fclose(outFile);//close output file

        ///////////////////unlock()///////////////////////
        pthread_mutex_unlock(&mutex_1);

        if(strlen(buffer) < 1024) //LOOP ENDING condition...
            eof = true;

        close(sock_fd); 
        printf("package %d completed", idx);
    }

    return 0;
}
//menu
void prompt()
{
	int a;
	printf("\nPlease choose from the following options: \n0: exit \n1: connect & download!\n2: about this software ");
	scanf("%d", &a);
	if(a == 0)
	{
		printf("\nThanks for choosing THE TYRATE BAY. Have a safe, and productive, day! ");
		exit(1);
	}
	else if (a == 1)
	{
		//main()
	}
	else if(a == 2)
	{
		info();
	}
	else
	{
		printf("\ncommand not recognized!");
		prompt();
	}
}
//displays program info
void info()
{
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n                                            Written and directed by: TYLER RABORN                                            \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	prompt();
}
//error handling
void error(const char * msg)
{
	perror(msg);
    exit(0);
}
