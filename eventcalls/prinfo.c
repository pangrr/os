/*
 * linux/kernel/prinfo.c
 */

#include <linux/prinfo.h>
#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/signal.h>

/*  
 *  Compare the values of two timespec variable.
 *  Return -1 if ts1 is less than ts2, 1 if greater, and 0 if they are equal.
 */
int cmp(const struct timespec ts1, const struct timespec ts2)
{
	if (ts1.tv_sec == ts2.tv_sec) {
		/* if seconds are equal, compare nano seconds */
		if (ts1.tv_nsec == ts2.tv_nsec)
			return 0;
		else if (ts1.tv_nsec < ts2.tv_nsec)
			return -1;
		else return 1;
	} else if (ts1.tv_sec < ts2.tv_sec)
		return -1;
	else return 1;
}


asmlinkage long sys_prinfo(struct prinfo * u_info)
{
	/* check whether u_info is a null pointer
 	 * return EINVAL if input pointer points to NULL */
	if (u_info == NULL)
		return EINVAL;
	
	/* allocate memory for struct prinfo
 	 * return EFAULT if allocation failed */
	struct prinfo *info = kmalloc(sizeof(struct prinfo), GFP_KERNEL);
	if (info == NULL)
		return EFAULT;
	
	/* copy prinfo from user space to kernel space
 	 * return EFAULT if copy failed */
	if (copy_from_user(info, u_info, sizeof(struct prinfo)) != 0) {
		/* free memory allocated to *info */
		kfree(info);
		return EFAULT;
	}

	/* search the process whose pid equals to info->pid */
	struct task_struct *p;
	for (p = &init_task; (p = next_task(p)) != &init_task;) {	
		if (info->pid == p->pid) {	
			/* state */
			info->state = p->state;
			/* parent pid, real parent pid */
			info->parent_pid = p->parent->pid;
			//info->real_parent_pid = p->real_parent->pid;
			/* start time */
			info->start_time = p->start_time;
			//info->real_start_time = p->real_start_time;
			/* user_time, sys_time */
			info->user_time = p->utime;
			info->sys_time = p->stime;
			/* uid */
			info->uid = p->real_cred->uid;
			/* comm */
			int i;
			for (i = 0; i < 15; i++)
				info->comm[i] = p->comm[i];
			info->comm[15] = '\0';

			/* cutime, cstime, youngest_child_pid */
			info->cutime = 0;
			info->cstime = 0;
			info->youngest_child_pid = -1;
			
			/* start time of youngest child */
			struct timespec youngest_start_time;
			
			/* find the youngest child */
			struct task_struct *pchild;
			list_for_each_entry(pchild, &(p->children), sibling) {
				/* sanity check
 				 * if the process's parent is some other process, do not take this process into consideration */
				if (pchild->parent != p) continue;
				
				/* compute children's user time and system time */
				info->cutime += pchild->utime;
				info->cstime += pchild->stime;
				
				/* pid of the youngest child */
				if (info->youngest_child_pid == -1 || cmp(pchild->start_time, youngest_start_time) > 0 ) {
					youngest_start_time = pchild->start_time;
					info->youngest_child_pid = pchild->pid;
				}
			}



			/* younger_sibling_pid, older_sibling_pid 
 			 * initialize them to -1 */
			info->younger_sibling_pid = -1;
			info->older_sibling_pid = -1;
			
			/*
			 * start times of the oldest child among the youngers 
 			 * and the youngest among the olders
 			 */ 
			struct timespec younger_start_time; 
			struct timespec older_start_time;
			struct task_struct *psibling;
			list_for_each_entry(psibling, &(p->sibling), sibling) {
				/* sanity check
 				 * if the process does not share the same parent, do not take this process into consideration */
				if (psibling->parent != p->parent) continue;
				
				/* find the pid of the oldest sibling among the younger siblings */
				if (cmp(psibling->start_time, p->start_time) > 0) {
					if (info->younger_sibling_pid == -1 || cmp(psibling->start_time, younger_start_time) < 0) {	
						info->younger_sibling_pid = psibling->pid;
						younger_start_time = psibling->start_time;
					}
				  /* find the pid of the youngest sibling among the older siblings */	
				} else if (cmp(psibling->start_time, p->start_time) < 0) {
					if (info->older_sibling_pid == -1 || cmp(psibling->start_time, older_start_time) > 0) {
						info->older_sibling_pid = psibling->pid;
						older_start_time = psibling->start_time;
					}	
				}
			}


			/* signal */
			info->signal = p->signal->shared_pending.signal.sig[0];
			
			/* copy prinfo from kernel space back to user space 
 			 * if copy failed return EFAULT */		
			if (copy_to_user(u_info, info, sizeof(struct prinfo)) != 0) {
				/* free memory space allocated to *info */
				kfree(info);
				return EFAULT;
			}
			/* free memory space allocated to *info */
			kfree(info);	
			return 0;
		}	
	}
	/* free memory space allocated to *info */
	kfree(info);
	/* if the process with pid info->pid does not exist, return EINVAL */
	return EINVAL;	
}
