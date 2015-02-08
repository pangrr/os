/* linux/kernel/MMU.c */

#include <linux/linkage.h>
#include <linux/types.h>    /* unitptr_t */
#include <linux/kernel.h>   /* printk() */   
#include <asm/uaccess.h>    /* copy_to_user() */
#include <asm/page.h>   /* __va() __pa() */
#include <asm/tlbflush.h>   /* flush_tlb_all() */

/*
 * Function: get_pagetable()
 *
 * Description:
 *  Return a physical address that can be used to access the current page
 *  table.
 */
static inline unsigned char *
get_pagetable (void) {
  /* Value of the CR3 register */
  uintptr_t cr3;

  /* Get the page table value out of CR3 */
  __asm__ __volatile__ ("movq %%cr3, %0\n" : "=r" (cr3));

  /*
   * Shift the value over 12 bits.  The lower-order 12 bits of the page table
   * pointer are assumed to be zero, and so they are reserved or used by the
   * hardware.
   */
  return (unsigned char *)((((uintptr_t)cr3) & 0x000ffffffffff000u));
}





/**
 * Function: sys_readMMU()
 *
 * Description:
 *  Store the value of the page table entry for virtual address vaddr into
 *  the memory pointed to by pte.
 *  Return 0 on success and -1 on error.
 *  Erros include virtual addresses which have no entry because a higher-level
 *  page table page is missing.
 */
asmlinkage long sys_readMMU(unsigned long vaddr, unsigned long *pte_u)
{
//    printk("vaddr = %lx\n", vaddr);


    /* 47:39 bits of the linear address */
    unsigned long pml4 = vaddr & 0x0000ff8000000000u/* 47:39 */;
    /* 51:12 bits of CR3 */
    unsigned char *cr3 = get_pagetable();
    /* Physical address of pml4e */
    unsigned long *_pml4e = (unsigned long *)((pml4 >> 36) | (uintptr_t)cr3);
    /* Value of pml4e */
    unsigned long pml4e = *((unsigned long *)(__va(_pml4e)));


//    printk("_pml4e = %lx\n", (unsigned long)_pml4e);
//    printk("pml4e = %lx\n", pml4e);


    /* Check pml4e present bit */
    if ((pml4e & 0x0000000000000001u) == 0x0000000000000000u) {
        printk("pml4e: not present\n");
        return -1;
    }








    /* 38:30 bits of the linear address */
    unsigned long dirp = vaddr & 0x0000007fc0000000u/* 38:30 */;
    /* Physical address of pdpte */
    unsigned long *_pdpte = (unsigned long *)((dirp >> 27) | (pml4e & 0x000ffffffffff000u/* 51:12 */));
    /* Value of pdpte */
    unsigned long pdpte = *((unsigned long *)(__va(_pdpte)));



//    printk("_pdpte = %lx\n", (unsigned long)_pdpte);    
//    printk("pdpte = %lx\n", pdpte);
    


    /* Check pdpte present bit */
    if ((pdpte & 0x0000000000000001u) == 0x0000000000000000u) {
        printk("pdpte: not present\n");
        return -1;
    }

    /* Check pdpte PS flag */
    if ((pdpte & 0x0000000000000080u) == 0x0000000000000080u) {
            /* If PS flag is 1, copy pdpte to user program */
            if (copy_to_user(pte_u, &pdpte, sizeof(unsigned long)) > 0) {
            printk("error copy_to_user\n");
            return -1;
        }
        return 0;
    }








    /* 29:21 bits of the linear address */
    unsigned long dir = vaddr & 0x000000003fe00000u/* 29:21 */;
    /* Physical address of pde */
    unsigned long *_pde = (unsigned long *)
        ((pdpte & 0x000ffffffffff000u/* 51:12 */)  | (dir >> 18));
    /* Value of pde */
    unsigned long pde = *((unsigned long *)(__va(_pde)));

//    printk("_pde = %lx\n", (unsigned long)_pde);    
//    printk("pde = %lx\n", pde);


    /* Check pde present bit */
    if ((pde & 0x0000000000000001u) == 0x0000000000000000u) {
        printk("pde: not present\n");
        return -1;
    }

    /* Check pde PS flag */
    if ((pde & 0x0000000000000080u) == 0x0000000000000080u) {
        /* If PS flag is 1, copy pde to user program */
        if(copy_to_user(pte_u, &pde, sizeof(unsigned long)) > 0) {
            printk("error copy_to_user\n");
            return -1;
        }
        return 0;
    }


    /* 20:12 bits of the linear address */
    unsigned long table = vaddr & 0x00000000001ff000u/* 20:12 */;
    /* Physical address of pte */
    unsigned long *_pte = (unsigned long *)
        ((pde & 0x000ffffffffff000u/* 51:12 */) | (table >> 9));
    /* Value of pte */
    unsigned long pte = *((unsigned long *)(__va(_pte)));


 
//    printk("_pte = %lx\n", (unsigned long)_pte);    
//    printk("pte = %lx\n", pte);


    /* Check pte present bit */
    if ((pte & 0x0000000000000001u) == 0x0000000000000000u) {
        printk("pte: not present\n");
        return -1;
    }

    /* Copy pte to user program */
    if (copy_to_user(pte_u, &pte, sizeof(unsigned long)) > 0) {
            printk("error copy_to_user\n");
            return -1;
    }
    
    return 0;
}









/**
 * Function sys_writeMMU()
 *
 * Description:
 *  Store the value of the page table entry value pte into the page table entry
 *  for virtual address vaddr.
 *  Return 0 on success and -1 on error.
 *  Errors include birtual addresses which have no entry because a higher-level
 *  page table is missing.
 */
asmlinkage long sys_writeMMU(unsigned long vaddr, unsigned long pte_u)
{
//    printk("vaddr = %lx\n", vaddr);
 


    /* 47:39 bits of the linear address */
    unsigned long pml4 = vaddr & 0x0000ff8000000000u/* 47:39 */;
    /* 51:12 bits of CR3 */
    unsigned char *cr3 = get_pagetable();
    /* Physical address of pml4e */
    unsigned long *_pml4e = (unsigned long *)((pml4 >> 36) | (uintptr_t)cr3);
    /* Value of pml4e */
    unsigned long pml4e = *((unsigned long *)(__va(_pml4e)));



//    printk("_pml4e = %lx\n", (unsigned long)_pml4e);
//    printk("pml4e = %lx\n", pml4e);




    /* Check pml4e present bit */
    if ((pml4e & 0x0000000000000001u) == 0x0000000000000000u) {
        printk("pml4e: not present\n");
        return -1;
    }





    /* 38:30 bits of the linear address */
    unsigned long dirp = vaddr & 0x0000007fc0000000u/* 38:30 */;
    /* Physical address of pdpte */
    unsigned long *_pdpte = (unsigned long *)((dirp >> 27) | (pml4e & 0x000ffffffffff000u/* 51:12 */));
    /* Value of pdpte */
    unsigned long pdpte = *((unsigned long *)(__va(_pdpte)));




//    printk("_pdpte = %lx\n", (unsigned long)_pdpte);    
//    printk("pdpte = %lx\n", pdpte);
    



    /* Check pdpte present bit */
    if ((pdpte & 0x0000000000000001u) == 0x0000000000000000u) {
        printk("pdpte: not present\n");
        return -1;
    }
    
    /* Check pdpte PS flag */
    if ((pdpte & 0x0000000000000080u) == 0x0000000000000080u) {
        /* If pdpte PS flag is 1, write pte_u to pdpte */
        *(unsigned long *)(__va(_pdpte)) = pte_u;
        /* Flush tlb */
        flush_tlb_all();
        return 0;
    }





    /* 29:21 bits of the linear address */
    unsigned long dir = vaddr & 0x000000003fe00000u/* 29:21 */;
    /* Physical address of pde */
    unsigned long *_pde = (unsigned long *)
        ((pdpte & 0x000ffffffffff000u/* 51:12 */)  | (dir >> 18));
    /* Value of pde */
    unsigned long pde = *((unsigned long *)(__va(_pde)));




//    printk("_pde = %lx\n", (unsigned long)_pde);    
//    printk("pde = %lx\n", pde);





    /* Check pde present bit */
    if ((pde & 0x0000000000000001u) == 0x0000000000000000u) {
        printk("pde: not present\n");
        return -1;
    }
  
    /* Check pde PS flag */
    if ((pde & 0x0000000000000080u) == 0x0000000000000080u) {
        /* If pde PS flag is 1, write pde */
        *(unsigned long *)(__va(_pde)) = pte_u;
        /* Flush tlb */
        flush_tlb_all();
        return 0;
    }






    /* 20:12 bits of the linear address */
    unsigned long table = vaddr & 0x00000000001ff000u/* 20:12 */;
    /* Physical address of pte */
    unsigned long *_pte = (unsigned long *)
        ((pde & 0x000ffffffffff000u/* 51:12 */) | (table >> 9));
    /* Value of pte */
    unsigned long pte = *((unsigned long *)(__va(_pte)));
 



//    printk("_pte = %lx\n", (unsigned long)_pte);    
//    printk("pte = %lx\n", pte);





    /* Check pte present bit */
    if ((pte & 0x0000000000000001u) == 0x0000000000000000u) {
        printk("pte: not present\n");
        return -1;
    }

    /* Write pte */
    *(unsigned long *)(__va(_pte)) = pte_u;
    /* Flush tlb */
    flush_tlb_all();


    return 0;  
}



