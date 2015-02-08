/* test2.c */

#include <sys/mman.h> /* mmap() */
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <linux/unistd.h>
#include <linux/kernel.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
 * Description:
 *  This program use mmap() to map the given file to virtual memory,
 *  then try to read the value of the corresponding pte.
 */

int main(int argc, char *argv[])
{
    /* Value of the file discriptor */
    int fd;
    /* Value pointed to by the virtual address where the file is mapped to */
    volatile unsigned long *vaddr;
    /* Information of the file */
    struct stat sb;
    /* Value of pte */    
    unsigned long pte = 0;
    

    /* Check for arguments validility */
    if (argc != 2) {
        printf("usage: test2.o <input file>\n");
        return -1;
    }

    /* Open input file */
    if ((fd = open(argv[1], O_RDWR)) < 0) {
        printf("Cannot open %s for reading\n", argv[1]);
        return -1;
    }

    /* Find the size of the input file */
    if (fstat(fd, &sb) < 0) {
        printf("fstat error\n");
        return -1;
    }
    
    /* mmap the input file */
    if ((vaddr = mmap(NULL, sb.st_size, PROT_READ |
                    PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
        printf("mmap error\n");
        return -1;
    }

    printf("Immediately after mmap():\n");
    /* Call readMMU() */
    if (syscall(181, vaddr, &pte) != 0) {
        printf("error readMMU()\n");
//        return -1;
    }



    /* Force read from vaddr to make sure pte is present */
    unsigned long force_read = *vaddr;
    printf("After reading from the virtual address:\n");
    /* Call readMMU() */
    if (syscall(181, vaddr, &pte) != 0) {
        printf("error readMMU()\n");
        return -1;
    }
    printf("pte = 0x%lx\n", pte);
    


    /* Force write to vaddr to make sure pte write bit is set */
    *vaddr = 1;
    printf("After writing to the virtual address:\n");
    /* Call readMMU() */
    if (syscall(181, vaddr, &pte) != 0) {
        printf("error readMMU()\n");
        return -1;
    }
    printf("pte = 0x%lx\n", pte);
    return 0;
}

