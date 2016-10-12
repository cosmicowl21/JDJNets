#include <sys/types.h>
#include <unistd.h> //fork()
#include <stdio.h>
#include <stdlib.h> //exit()
#include <sys/wait.h> //wait()

int main(int ac, char *av[])
{

pid_t pids[10];
int i;
int n = 10;

//Start children */
for (i=0; i<n; i++)
{
if ((pids[i] = fork()) < 0)
{
perror("fork errror");
exit(-1);
} else if (pids[i] == 0)
{
printf("Im a child : %d, my parent is : %d\n",getpid(),getppid());
exit(0);
}
}

//Wait for children to exit
int status;
pid_t pid;
while (n>0)
{
pid = wait(&status);
printf("Child with PID %1d exited with status 0x%x.\n",(long)pid, status);
--n; //TODO(pts): Remove pid from pids array
}

return(0);
}

