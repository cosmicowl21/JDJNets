//James Walsh
#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread

/ Creates a string array and uses the words text file 
 int maxlives = 12;
 char *word [] = { 
 # include "words" // brings in the text file
 };

//defining the ports and the ammout of letters that can be used 
 # define NUM_OF_WORDS (sizeof (word) / sizeof (word [0]))
 # define MAXLEN 80 /* Maximum size in the world of Any string */
 # define HANGMAN_TCP_PORT 1067

 
//the thread function
void *connection_handler(void *);
 
int main(int argc , char *argv[])
{
// any coments that have hte word "same" are reffering to the program hangserver.c
 
     
    int socket_desc , client_sock , c , *new_sock,client_len;
	//socket_desc is the same as sock
	// client_sock is hte file descriptor
	// c is the client lenght
    struct sockaddr_in server , client; // this is the same
    
    srand ((int) time ((long *) 0)); /* randomize the seed */
     
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);//0 or IPPROTO_TCP
    if (socket_desc <0)
    {
        printf("Sorry the socket could not be created");
    }
    puts("Socket has been created"); 
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons( HANGMAN_TCP_PORT);
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error called");
        return 1;
    }
    puts("bind has been completed");
     
    //Listen
    listen(socket_desc , 3);
     
    //Accept and incoming connection
    puts("Waiting for clients to connect");
    c = sizeof(struct sockaddr_in);
     
     
    //Accept and incoming connection
    puts("Waiting for clients to connect");
    c = sizeof(struct sockaddr_in);
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Accepted the connection");
         
        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;
         
        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("Thread has not been created");
            return 1;
        }
         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        puts("Client has been given a handler");
    }
     
    if (client_sock < 0)
    {
        perror("accept has failed");
        return 1;
    }
     
    return 0;
}
 
/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char *message , client_message[2000];
     
    //Send some messages to the client
    message = "Hello this is your handler. Welcome to the server\n";
    write(sock , message , strlen(message));
     
    message = "Whatever you type i will repeat \n";
    write(sock , message , strlen(message));
     
    //Receive a message from client
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        //Send the message back to client
        write(sock , client_message , strlen(client_message));
    }
     
    if(read_size == 0)
    {
        puts("Client has been disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv has failed");
    }
         
    //Free the socket pointer
    free(socket_desc);
     
    return 0;
}
