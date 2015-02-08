/* test1.c */

 

#include <linux/unistd.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

/**
 * Description:
 *  This program calls the system call sys_readMMU() to read value of virtual
 *  address 0
 */
int main(int argc, char **argv)
{
    /* Initialize pte to an arbitrary unsigned long */
	unsigned long pte = 1234;
    if (syscall(181, 0, &pte) != 0) {
        printf("error readMMU()\n");
        return -1;
    }
    /* Print the value of pte on success */
    printf("pte = %lx\n", pte);
	return 0;
}

