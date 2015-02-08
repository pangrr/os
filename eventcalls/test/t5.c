#include <sys/syscall.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	int num,num2,i;
	int *p;
	
	/* Use doeventinfo to get the return number of events. */
	num = syscall(185,0,NULL);
	printf("Numbers of events: %d\n", num);
	p = malloc(num * sizeof(int));

	/* Use doventinfo to get the return events' id. */
	num2 = syscall(185,num,p);
	printf("Second return number: %d\n", num2);
	for(i = 0;i < num;i++) {
		printf("Event ID : %d\n", p[i]);
	}
	return 0;
}
