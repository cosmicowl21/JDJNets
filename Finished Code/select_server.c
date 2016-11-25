/* 
 * commands from stdin:
 *   "c<nl>"  print the number of connection requests
 *   "q<nl>"  quit the server 
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <signal.h>
#include <syslog.h>

 extern time_t time ();

#include <arpa/inet.h>

#define BUFSIZE 1024
# define MAXLEN 80
#define SELECT 8888
 int maxlives = 12;
 char *word [] =
 { 
 # include "words" // brings in the text file
 };
//defining the ports and the ammout of letters that can be used 
 # define NUM_OF_WORDS (sizeof (word) / sizeof (word [0]))
/*
 * error - wrapper for perror
 */
void error(char *msg) 
{
  perror(msg);
  exit(1);
}

int main(int argc, char **argv) 
{
  int parentfd; /* parent socket */
  int childfd; /* child socket */
  int portno; /* port to listen on */
  int clientlen; /* byte size of client's address */
  struct sockaddr_in serveraddr; /* server's addr */
  struct sockaddr_in clientaddr; /* client addr */
  char buf[BUFSIZE]; /* message buffer */
  int optval; /* flag value for setsockopt */
  int n; /* message byte size */
  int connectcnt; /* number of connection requests */
  int notdone;
  fd_set readfds;

  /* 
   * check command line arguments 
   */
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }
  portno = atoi(argv[1]);

  /* 
   * socket: create the parent socket 
   */
  parentfd = socket(AF_INET, SOCK_STREAM, 0);
  if (parentfd < 0) 
    error("ERROR opening socket");

  /* setsockopt: Handy debugging trick that lets 
   * us rerun the server immediately after we kill it; 
   * otherwise we have to wait about 20 secs. 
   * Eliminates "ERROR on binding: Address already in use" error. 
   */
  optval = 1;
  setsockopt(parentfd, SOL_SOCKET, SO_REUSEADDR, 
	     (const void *)&optval , sizeof(int));
printf("Socket is created \n");

  /*
   * build the server's Internet address
   */
  bzero((char *) &serveraddr, sizeof(serveraddr));
  printf("Servers Internet address has been built \n");	

  /* this is an Internet address */
  serveraddr.sin_family = AF_INET;

  /* let the system figure out our IP address */
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

  /* this is the port we will listen on */
  serveraddr.sin_port = htons((unsigned short)portno);

  /* 
   * bind: associate the parent socket with a port 
   */
  if (bind(parentfd, (struct sockaddr *) &serveraddr, 
	   sizeof(serveraddr)) < 0) 
    error("ERROR on binding \n");

    printf("Binding has finished \n");

  /* 
   * listen: make this socket ready to accept connection requests 
   */
  if (listen(parentfd, 5) < 0) /* allow 5 requests to queue up */ 
    error("ERROR on listen");
	
    printf("Listing is finnished \n ");

  /* initialize some things for the main loop */
  clientlen = sizeof(clientaddr);
  notdone = 1;
  connectcnt = 0;
  printf("Welcome to the server> \n");
  fflush(stdout);

  /* 
   * main loop: wait for connection request or stdin command.
   *
   * If connection request, then echo input line 
   * and close connection. 
   * If command, then process command.
   */
  while (notdone) 
{

    /* 
     * select: Has the user typed something to stdin or 
     * has a connection request arrived?
     */
    FD_ZERO(&readfds);          /* initialize the fd set */
    FD_SET(parentfd, &readfds); /* add socket fd */
    FD_SET(0, &readfds);        /* add stdin fd (0) */
    if (select(parentfd+1, &readfds, 0, 0, 0) < 0) {
      error("ERROR in select");
    }

    /* if the user has entered a command, process it */
    if (FD_ISSET(0, &readfds)) {
      fgets(buf, BUFSIZE, stdin);
      switch (buf[0]) {
      case 'c': /* print the connection cnt */
	printf("Received %d connection requests so far.\n", connectcnt);
	printf("server> ");
	fflush(stdout);
	break;
      case 'q': /* terminate the server */
	notdone = 0;
	break;
      default: /* bad input */
	printf("ERROR: unknown command\n");
	printf("server> ");
	fflush(stdout);
      }
    }    

    /* if a connection request has arrived, process it */
    if (FD_ISSET(parentfd, &readfds)) {
      /* 
       * accept: wait for a connection request 
       */
      childfd = accept(parentfd, 
		       (struct sockaddr *) &clientaddr, &clientlen);
      if (childfd < 0) 
	error("ERROR on accept");
      connectcnt++;
      
      /* 
       * read: read input string from the client
       */
      bzero(buf, BUFSIZE);
      n = read(childfd, buf, BUFSIZE);
      if (n < 0) 
	error("ERROR reading from socket");
      
      /* 
       * write: echo the input string back to the client 
       */
      n = write(childfd, buf, strlen(buf));
      if (n < 0) 
	error("ERROR writing to socket");
      
      close(childfd);
    }
  }

  /* clean up */
  printf("Terminating server.\n");
  close(parentfd);
  exit(0);
}




 /* ---------------- Play_hangman () ---------------------*/

 play_hangman (int in, int out)
 {
 	char * whole_word, part_word [MAXLEN],
 	guess[MAXLEN], outbuf [MAXLEN];
	printf("Playing hangman");
 	int lives = maxlives;
 	int game_state = 'I';//I = Incomplete
 	int i, good_guess, word_length;
 	char hostname[MAXLEN];

 	gethostname (hostname, MAXLEN);
 	sprintf(outbuf, "Playing hangman on host% s: \n \n", hostname);
 	write(out, outbuf, strlen (outbuf));

 	/* Pick a word at random from the list */
 	whole_word = word[rand() % NUM_OF_WORDS];
 	word_length = strlen(whole_word);
 	syslog (LOG_USER | LOG_INFO, "server chose hangman word %s", whole_word);

 	/* No letters are guessed Initially */
 	for (i = 0; i <word_length; i++)
 		part_word[i]='-';
 	
	part_word[i] = '\0';

 	sprintf (outbuf, "%s %d \n", part_word, lives);
 	write (out, outbuf, strlen(outbuf));

 	while (game_state == 'I')
 	/* Get a letter from player guess */
 	{
		while (read (in, guess, MAXLEN) <0) {
 			if (errno != EINTR)
 				exit (4);
 			printf ("re-read the startin \n");
 			} /* Re-start read () if interrupted by signal */
 	good_guess = 0;
 	for (i = 0; i <word_length; i++) {
 		if (guess [0] == whole_word [i]) {
 		good_guess = 1;
 		part_word [i] = whole_word [i];
 		}
 	}
 	if (! good_guess) lives--;
 	if (strcmp (whole_word, part_word) == 0)
 		game_state = 'W'; /* W ==> User Won */
 	else if (lives == 0) {
 		game_state = 'L'; /* L ==> User Lost */
 		strcpy (part_word, whole_word); /* User Show the word */
 	}
 	sprintf (outbuf, "%s %d \n", part_word, lives);
 	write (out, outbuf, strlen (outbuf));
 	}
 }
