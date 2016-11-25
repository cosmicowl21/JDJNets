 #include <errno.h>
 #include <sys/types.h>
 #include <netdb.h>
 #include <netinet/in.h>
 #include <stdlib.h>
 #include <string.h>
 #include <sys/socket.h>
 #include <stdio.h>
 #include <syslog.h>
 #include <signal.h>
 #include <unistd.h>

typedef void Sigfunc(int);
#define PORT 1066
#define MAXLEN 1024
	
extern time_t time ();

int maxlives = 12;
char *word [] =
{
	#include "words"
};
# define NUM_OF_WORDS (sizeof (word) / sizeof (word [0]))

Sigfunc *signal_setup(int signo, Sigfunc *func)
{
	struct sigaction act, oact;

	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (signo == SIGALRM)
	{
#ifdef	SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT;	/* SunOS 4.x */
#endif
	}
	else
	{
#ifdef	SA_RESTART
		act.sa_flags |= SA_RESTART;		/* SVR4, 44BSD */
#endif
	}
	if (sigaction(signo, &act, &oact) < 0)
	{
		return(SIG_ERR);
	}
	return(oact.sa_handler);
}

void sig_chld(int signo)
{
	pid_t pid;
	int stat;

	while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
	{
		printf("child %d terminated\n", pid);
	}
	return;
}


int main()
{
	srand((unsigned)time(0));
	int listen_sock, accept_sock, client_size;
	pid_t pid;
	struct sockaddr_in listen_sock_address_info, client_details;
	
	//create the socket that will listen for connections
	listen_sock = socket(AF_INET, SOCK_STREAM, 0);

	if(listen_sock < 0)
	{
		perror("Creation of listen_sock failed");
		exit(1);
	}

	//define the listening sockets' details
	listen_sock_address_info.sin_family = AF_INET;
 	listen_sock_address_info.sin_addr.s_addr = htonl(INADDR_ANY);
 	listen_sock_address_info.sin_port = htons(PORT);
	
	if (bind(listen_sock, (struct sockaddr*) &listen_sock_address_info, sizeof(listen_sock_address_info)) < 0)
	{
 		perror("Bind call for listen_sock failed");
	 	exit(1);
 	}

 	listen (listen_sock, 5);
	client_size = sizeof(client_details);
	signal_setup(SIGCHLD, sig_chld);
	
	while(1)
	{
		if(accept_sock = accept(listen_sock, (struct sockaddr *) &client_details, &client_size) < 0)
		{

			printf("Accept call failed");			
			exit(3);
		}
		else
		{	//the parent forks a child
			//the parent has a non-zero id
			//and the child has an id of 0
			pid = fork();
			if(pid == 0)
			{	// if this is the child process, stop listening and play!
				close(listen_sock);
				play_hangman(accept_sock, accept_sock);
				// when finished playing, close connection
				close(accept_sock);
				exit(0);
			}
			else
			{
				//if you are the parent, listen for more guests
				close(accept_sock);
			}
		}
			
		
	}
	//never reached but will close the parents' listening socket
	close(listen_sock);
}

play_hangman (int in, int out)
 {
 	char * whole_word, part_word [MAXLEN],
 	guess[MAXLEN], outbuf [MAXLEN];

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
		while (read (in, guess, MAXLEN) <0)
		{
 			if (errno != EINTR)
			{
 				exit (4);
 			}
			printf ("re-read the startin \n");
		} /* Re-start read () if interrupted by signal */
 	good_guess = 0;
 	for (i = 0; i <word_length; i++)
	{
 		if (guess [0] == whole_word [i])
		{
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
