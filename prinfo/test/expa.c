/*
 * This program prints out its process id and then sleep for 100 seconds
 */

#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <stddef.h>

int main(int argc, char **argv)
{
	pid_t pid = getpid();
	printf ("pid: %d\n", pid);
	sleep (100);
	return 0;
}
