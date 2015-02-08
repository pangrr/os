/* test3.c */

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

/* Description:
 *  This program user mmap() to map two different files into two different
 *  virtual addresses; call these virtual addresses A and B.
 *  This program then use readMMU() and writeMMU() to map the physical page
 *  mapped to virtual address A to virtual address B.
 *  This program then write into virtual address A and check if the same value
 *  is written into virtual address B.
 */

int main(int argc, char *argv[])
{
    /* File descriptors for file1 and file2 */
    int fd1, fd2;
    /* Values pointed to by the virtual addresses where file1 and file2 are
     * mapped to */
    volatile unsigned long *vaddr1, *vaddr2;
    /* Information of file1 and file2 */
    struct stat sb1, sb2;

    /* Check input arguments */
    if (argc != 3) {
        printf("usage: test3.o <input file 1> <input file 2>\n");
        return -1;
    }




    /* Open input file1 */
    if ((fd1 = open(argv[1], O_RDWR)) < 0) {
        printf("Cannot open %s for reading\n", argv[1]);
        return -1;
    }
    /* Find the size of the input file1 */
    if (fstat(fd1, &sb1) < 0) {
        printf("fstat error\n");
        return -1;
    }
    /* mmap file1 */
    if ((vaddr1 = mmap(NULL, sb1.st_size, PROT_READ |
                    PROT_WRITE, MAP_SHARED, fd1, 0)) == MAP_FAILED) {
        printf("mmap error\n");
        return -1;
    }



    /* Open input file2 */
    if ((fd2 = open(argv[2], O_RDWR)) < 0) {
        printf("Cannot open %s for reading\n", argv[2]);
        return -1;
    }
    /* Find the size of the input file2 */
    if (fstat(fd2, &sb2) < 0) {
        printf("fstat error\n");
        return -1;
    } 
    /* mmap file2 */
    if ((vaddr2 = mmap(NULL, sb2.st_size, PROT_READ |
                    PROT_WRITE, MAP_SHARED, fd2, 0)) == MAP_FAILED) {
        printf("mmap error\n");
        return -1;
    }


    /* Force read from virtual address to make sure pte is present */
    unsigned long force_read1 = *vaddr1;
    unsigned long force_read2 = *vaddr2;
//    printf("force_read1 = %lu\n", force_read1);
//    printf("force_read2 = %lu\n", force_read2);
    
    /* Force write to virtual address to make sure pte read/write bit is set */
    *vaddr1 = 1;
    *vaddr2 = 2;
    printf("*vaddr1 is %lu.\n", (unsigned long)*vaddr1);
    printf("*vaddr2 is %lu.\n", (unsigned long)*vaddr2);

    /* Read pte from file1 */
    unsigned long pte1 = 0;
    if (syscall(181, vaddr1, &pte1) != 0) {
        printf("error readMMU()\n");
        return -1;
    }
//    printf("pte1 = %lx\n", pte1);
    

    /* Read pte from file2 to restore it later */
    unsigned long pte2 = 0;
    if (syscall(181, vaddr2, &pte2) != 0) {
        printf("error readMMU()\n");
        return -1;
    }
    
//    printf("pte2 = %lx\n", pte2);
    

    /* Write pte of file2 */
    if (syscall(182, vaddr2, pte1) != 0) {
        printf("error write pte\n");
        return -1;
    }
    printf("pte of file 2 has been changed to pte of file 1.\n");
    
    
    /* Check if we do it seccessfully */
    *vaddr1 = 100;
    printf("*vaddr1 is now changed to %lu.\n", (unsigned long)*vaddr1);
    printf("*vaddr2 is now %lu\n", (unsigned long)*vaddr2);


    /* Restore pte of file2 before we leave */
    if (syscall(182, vaddr2, pte2) != 0) {
        printf("error write pte\n");
        return -1;
    }

    return 0;
}


