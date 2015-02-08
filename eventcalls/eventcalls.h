/* linux/include/linux/eventcalls.h */

#ifndef __EVENT_H
#define __EVENT_H

#include <linux/kernel_stat.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/percpu.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/swap.h>
#include <linux/pid_namespace.h>
#include <linux/notifier.h>
#include <linux/thread_info.h>
#include <linux/time.h>
#include <linux/jiffies.h>
#include <linux/posix-timers.h>
#include <linux/cpu.h>
#include <linux/syscalls.h>
#include <linux/delay.h>
#include <linux/tick.h>
#include <linux/kallsyms.h>

#include <asm/uaccess.h>
#include <asm/unistd.h>
#include <asm/div64.h>
#include <asm/timex.h>
#include <asm/io.h>

#include <linux/kernel.h>

#include <linux/sched.h>   
#include <linux/list.h> 
#include <linux/wait.h>
#include <linux/spinlock.h>
#include <linux/types.h>

struct event
{
    uid_t UID;
    gid_t GID;
    int UIDFlag;
    int GIDFlag;
    /* eventID should be positive integers. */
    int eventID;    
    /* Implement a kernel double-linked list fo events. */
    struct list_head eventID_list;
    /* Implement a wait queue of processes waiting on the event. */
    wait_queue_head_t wait_queue;

};




/*
 * Return the length of the list with given list_head.
 * Remember to call read_lock before.
 */
int get_list_length(struct list_head * head);   



/*
 * Return a pointer to the event with given event ID.
 * Return NULL if the event with the given event ID is not found.
 * Remember to call read_lock before.
 */
struct event * get_event(int eventID);




/*
 * Initialize a global event as the head of a linked list of other events.
 * This function should be called in function start_kernel() in linux/init/main.c at kernel boot.
 */
void doevent_init();




/* 181
 * Create a new event and assign an event ID to it.
 * Add the new event to the global event list.
 * Return event id on success.
 * Return -1 on failure.
 */
asmlinkage long sys_doeventopen();




/* 182
 * Wake up all tasks in the waiting queue of the event with given eventID.
 * Remove the event from global event list.
 * Free memory which hold the event.
 * Return the number of processed signaled on success.
 * Return -1 on failure.
 * Access denied:
 *  uid != 0 && (uid != event->UID || event->UIDFlag == 0) && (gid != event->GID || event->GIDFlag == 0)
 */
asmlinkage long sys_doeventclose(int eventID);




/* 183
 * Make the calling tasks wait in the wait queue of the event with the given eventID.
 * Rreturn 0 on success.
 * Return -1 on failure.
 * Access denied:
 *  uid != 0 && (uid != event->UID || event->UIDFlag == 0) && (gid != event->GID || event->GIDFlag == 0)
 */
asmlinkage long sys_doeventwait(int eventID);




/* 184
 * Wake up all tasks waiting in the event with the given event ID.
 * Remove all tasks from waiting queue.
 * Return the number of processes signaled on success.
 * Return -1 on failure.
 * Access denied:
 *  uid != 0 && (uid != event->UID || event->UIDFlag == 0) && (gid != event->GID || event->GIDFlag == 0)
 */
asmlinkage long sys_doeventsig(int eventID);




/* 185
 * If eventIDs != NULL && num >= event_count, copy all event IDs to user array pointed to by eventIDs.
 * If eventIDs == NULL do not copy.
 * Return the number of active events on success.
 * Return -1 if num is less than the number of events or on failure.
 */
asmlinkage long sys_doeventinfo(int num, int * eventIDs);




/* 205
 * Change the UID and GID of the event with the given eventID to the given UID and GID.
 * Return 0 on success.
 * Return -1 on failure.
 * Access denied:
 *  uid != 0 && uid != event->UID
 */
asmlinkage long sys_doeventchown(int eventID, uid_t UID, gid_t GID);




/* 211
 * Change event->UIDFlag to UIDFlag and event->GIDFlag to GIDFlag.
 * Return 0 on success.
 * Retutn -1 on failure.
 * Access denied:
 *  uid != 0 && uid != event->UID
 */
asmlinkage long sys_doeventchmod(int eventID, int UIDFlag, int GIDFlag);




/* 214
 * Place the UID, GID, User Signal Enable Bit and Group Signal Enable Bit into the memory pointed to by UID, GID, UIDFlag and GIDFlag respectively.
 * Return 0 on success.
 * Return -1 on filure.
 */
asmlinkage long sys_doeventstat(int eventID, uid_t * UID, gid_t * GID, int * UIDFlag, int * GIDFlag);


extern rwlock_t eventID_list_lock;  //provide read write lock to evnetID list
extern struct event global_event;   //provide the main list
extern bool event_initialized;  //indicate if the global event has been initialized

#endif
