#include <stdio.h>
#include <sys/types.h>

#define void child();
#define void parent();

int main()
{
	pid_t id; //signed int which can represent a process ID
	
	id = fork(); //makes two processes, a child and parent
	
	
	// all children's ids will be 0
	if(id == 0)
	{
		child();
	}
	
	// any other value means it is a parent
	else
	{
		parent();
	}
}

void child()
{
	printf("child");
}

void parent()
{
	printf("parent");
}