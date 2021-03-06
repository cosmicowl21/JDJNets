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

#define PORT 1066
#define MAXLEN 1024

int maxlives = 12;
char usernameStorage[MAXLEN][MAXLEN];
char hangmanWord[MAXLEN];
char guessedHangmanWord[MAXLEN];
int sock;
	
char *word [] =
{
	#include "words"
};
# define NUM_OF_WORDS (sizeof (word) / sizeof (word [0]))

int main()
{
	int check = 1, users = 0;
	char username[MAXLEN];
	char otherstuff[MAXLEN];
	char makeItWork[MAXLEN];
	struct sockaddr_in sock_address_info, clientAddressInfo;
	
	socklen_t len;
	
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	sock_address_info.sin_family = AF_INET;
 	sock_address_info.sin_addr.s_addr = htonl(INADDR_ANY);
 	sock_address_info.sin_port = htons(PORT);
	bind(sock, (struct sockaddr*) &sock_address_info, sizeof sock_address_info);
	len = sizeof sock_address_info;
	
	while(1)
	{
		check = 1;
		recvfrom(sock, username, MAXLEN, 0, &clientAddressInfo, &len);
		
		printf("%s\n", username);
		int i, j;
		for(i = 0; i < MAXLEN; i++)
		{
			for(j = 0; j < strlen(username); j++)
			{
				if(username[j] != usernameStorage[i][j])
				{
					check = 0;
					j = strlen(username);
				}
				else
				{
					check = 1;
				}
			}
			if(check == 1)
			{
				i = MAXLEN;
			}
		}

		if(check == 0)
		{
			hangmanSetup(username, users);
			users++;
			printf("New User");
		}
		else
		{
		//	hangman(sock_address_info);
			printf("Old User");
		}
		sendto(sock, "Thank you, come again.", MAXLEN, 0, &clientAddressInfo, sizeof clientAddressInfo);
	}
	close(socket);
}



int hangmanSetup(char username[], int index)
{
/*	char * whole_word, part_word [MAXLEN],
 	guess[MAXLEN], outbuf [MAXLEN];

 	int lives = maxlives;
 	int game_state = 'I';//I = Incomplete
 	int i, good_guess, word_length;
 	char hostname[MAXLEN];

 	gethostname (hostname, MAXLEN);
 	sprintf(outbuf, "Playing hangman on host% s: \n \n", hostname);
 	write(out, outbuf, strlen (outbuf));

 	/* Pick a word at random from the list */
 /*	whole_word = word[rand() % NUM_OF_WORDS];
 	word_length = strlen(whole_word);
 	syslog (LOG_USER | LOG_INFO, "server chose hangman word %s", whole_word);

 	/* No letters are guessed Initially */
/* 	for (i = 0; i <word_length; i++)
	{
 		part_word[i]='-';
		guessedHangmanWord[i] = '-';
 	}
	part_word[i] = '\0';
	guessedHangmanWord[i] = '\0';
*/
	int i;
	for(i = 0; i < strlen(username); i++)
	{
		if(username[i] != '\0')
		{
			usernameStorage[index][i] = username[i];
		}
	}
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
 	sprintf(outbuf, "Playing hangman on host%s: \n \n", hostname);
 	write(out, outbuf, strlen (outbuf));

 	/* Pick a word at random from the list */
 	whole_word = word[rand() % NUM_OF_WORDS];
 	word_length = strlen(whole_word);
 	syslog (LOG_USER | LOG_INFO, "server chose hangman word %s", whole_word);

 	/* No letters are guessed Initially */
 	for (i = 0; i <word_length; i++)
	{
 		part_word[i]='-';
		guessedHangmanWord[i] = '-';
 	}
	part_word[i] = '\0';
	guessedHangmanWord[i] = '\0';

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
	 	if (! good_guess)
		{
			lives--;
		}

	 	if (strcmp (whole_word, part_word) == 0)
	 	{
			game_state = 'W'; /* W ==> User Won */
	 	}
		else if (lives == 0)
		{
	 		game_state = 'L'; /* L ==> User Lost */
	 		strcpy (part_word, whole_word); /* User Show the word */
	 	}
	 	sprintf (outbuf, "%s %d \n", part_word, lives);
	 	write (out, outbuf, strlen (outbuf));
 	}
 }
