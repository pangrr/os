/* linux/kernel/eventcalls.c */


#include <linux/eventcalls.h>


/* Event list head with eventID is 0 and is initilized during kernel boot. */
struct event global_event;
/* A read write lock on event list. */
rwlock_t eventID_list_lock;
/* A state indicating whether the global_event has been initialized successfully. */
bool event_initialized;






/*
 * Return the length of the list with given list_head.
 * Remember to call read_lock before.
 */
int get_list_length(struct list_head * head)
{
    /* Check arguemnts. */
    if (head == NULL) {
        printk("error get_list_length(): invalid argument\n");
        return -1;
    }

    int length = 0;
    struct list_head * pos;
    list_for_each(pos, head) length++;
    return length;
}







/*
 * Return a pointer to the event with given event ID.
 * Return NULL if the event with the given event ID is not found.
 * Remember to call read_lock before.
 */
struct event * get_event(int eventID)
{
    /* Remember to check if event is initialized at kernel boot before actually doing anything. */
    if (event_initialized == false) {
        printk("error get_event(): event not initialized\n");
        return -1;
    }

    
    struct event * pos;
    list_for_each_entry(pos, &global_event.eventID_list, eventID_list) {
        if (pos->eventID == eventID) {
            return pos;
        }
    }


    return (struct event *) NULL;
}








/*
 * Initialize a global event as the head of a linked list of other events.
 * Set event_initialized true.
 * This function should be called in function start_kernel() in linux/init/main.c at kernel boot.
 */
void doevent_init()
{
    eventID_list_lock = RW_LOCK_UNLOCKED;

    INIT_LIST_HEAD(&global_event.eventID_list);

    global_event.eventID = 0;

//    global_event.wait_queue_lock = RW_LOCK_UNLOCKED;

    init_waitqueue_head(&global_event.wait_queue);
    event_initialized = true;
}







/*
 * Create a new event and assign an event ID to it.
 * Add the new event to the global event list.
 * Return event id on success.
 * Return -1 on failure.
 */
asmlinkage long sys_doeventopen()
{

    /* Remember to check if event is initialized at kernel boot before actually doing anything. */
    if (event_initialized == false) {
        printk("error sys_doeventopen(): event not initialized\n");
        return -1;
    }

    struct event * new_event = kmalloc(sizeof(struct event), GFP_KERNEL);

    /* Initialize attributes of new_event. */
    new_event->UID = current->cred->euid;  
    new_event->GID = current->cred->egid;
    new_event->UIDFlag = 1;
    new_event->GIDFlag = 1;
//  new_event->wait_queue_lock = RW_LOCK_UNLOCKED;
    
    /* Initialize event list entry. */
    INIT_LIST_HEAD(&(new_event->eventID_list));

    unsigned long flags;
    /* Lock write on event list. */
    write_lock_irqsave(&eventID_list_lock, flags);
    /* Add new_event to the tail of event list. */
    list_add_tail(&(new_event->eventID_list), &global_event.eventID_list);
    /* Find immediate preceding event's ID. */
    int max_id = list_entry((new_event->eventID_list).prev, struct event, eventID_list)->eventID;
    /* Assign eventID to new_event. No duplicate! */
    new_event->eventID = max_id + 1;
    /* Initialize wait queue. */
    init_waitqueue_head(&(new_event->wait_queue)); 
    write_unlock_irqrestore(&eventID_list_lock, flags);
    /* Write unlocked on event list. */


    return new_event->eventID;
}






/*
 * Wake up all tasks in the waiting queue of the event with given eventID.
 * Remove the event from global event list.
 * Free memory which hold the event.
 * Return the number of processed signaled on success.
 * Return -1 on failure.
 * Access denied:
 *  uid != 0 && (uid != event->UID || event->UIDFlag == 0) && (gid != event->GID || event->GIDFlag == 0)
 */
asmlinkage long sys_doeventclose(int eventID)
{
    /* Remember to check if event is initialized at kernel boot before actually doing anything. */
    if (event_initialized == false) {
        printk("error sys_doeventclose(): event not initialized\n");
        return -1;
    }    


    unsigned long flags;
    /* Lock read. */
    read_lock_irqsave(&eventID_list_lock, flags);
    /* Search for event in event list. */
    struct event * this_event = get_event(eventID);
    read_unlock_irqrestore(&eventID_list_lock, flags);
    /* Read unlocked. */

    
    /* If event not found. */
    if (this_event == NULL) {
        printk("error sys_doeventclose(): event not found. eventID = %d\n", eventID);
        return -1;
    }

    /* Check accessibility. */
    uid_t uid = current->cred->euid;
    gid_t gid = current->cred->egid;
    if (uid != 0 && (uid != this_event->UID || this_event->UIDFlag == 0) && (gid != this_event->GID || this_event->GIDFlag == 0)) {
        printk("sys_doeventclose(): access denied\n");
        return -1;
    }


    /*
     * Wake up all tasks in the waiting queue of the event.
     * Remove the tasks from the waiting queue.
     */
    int processes_signaled = sys_doeventsig(eventID);

   
    /* Lock write. */
    write_lock_irqsave(&eventID_list_lock, flags);
    /* Delete event from event list. */
    list_del(&(this_event->eventID_list));
    write_unlock_irqrestore(&eventID_list_lock, flags);
    /* Write unlocked. */

    /* Remember to free memory. */
    kfree(this_event);
    return processes_signaled;
}






/*
 * Make the calling tasks wait in the wait queue of the event with the given eventID.
 * Rreturn 0 on success.
 * Return -1 on failure.
 * Access denied:
 *  uid != 0 && (uid != event->UID || event->UIDFlag == 0) && (gid != event->GID || event->GIDFlag == 0)
 */
asmlinkage long sys_doeventwait(int eventID)
{

    /* Remember to check if event is initialized at kernel boot before actually doing anything. */
    if (event_initialized == false) {
        printk("error sys_doeventwait(): event not initialized\n");
        return -1;
    }


    unsigned long flags;
    /* Lock read. */
    read_lock_irqsave(&eventID_list_lock, flags);
    /* Search for the event in the event list. */
    struct event * this_event = get_event(eventID);
    read_unlock_irqrestore(&eventID_list_lock, flags);
    /* Read unlocked. */

    /* If event not found. */
    if (this_event == NULL) {
        printk("error sys_doeventwait(): event not found. eventID = %d\n", eventID);
        return -1;
    }


    /* Check accessibility. */
    uid_t uid = current->cred->euid;
    gid_t gid = current->cred->egid;
    if (uid != 0 && (uid != this_event->UID || this_event->UIDFlag == 0) && (gid != this_event->GID || this_event->GIDFlag == 0)) {
        printk("sys_doeventwait(): access denied\n");
        return -1;
    }

    

    DEFINE_WAIT(wait);
    /* 
     * Lock wait_queue so that no other process can wait on or wake up the wait queue,
     * until this process has changed its status.
     */
    /* Change task status to either TASK_INTERRUPTIBLE or TASK_UNINTERRUPTIBLE. */
    prepare_to_wait(&(this_event->wait_queue), &wait, TASK_INTERRUPTIBLE);
    /* 
     * Wait queue has been unlocked.
     * Other process can wait on this queue or wake up tasks on this queue.
     */

    schedule();
    finish_wait(&(this_event->wait_queue), &wait);


    return 0;
}






/*
 * Wake up all tasks waiting in the event with the given event ID.
 * Remove all tasks from waiting queue.
 * Return the number of processes signaled on success.
 * Return -1 on failure.
 * Access denied:
 *  uid != 0 && (uid != event->UID || event->UIDFlag == 0) && (gid != event->GID || event->GIDFlag == 0)
 */
asmlinkage long sys_doeventsig(int eventID)
{

    /* Remember to check if event is initialized at kernel boot before actually doing anything. */
    if (event_initialized == false) {
        printk("error sys_doeventsig(): event not initialized\n");
        return -1;
    }

    unsigned long flags;
    /* Lock read. */
    read_lock_irqsave(&eventID_list_lock, flags);
    /* Search for the event in the event list. */
    struct event * this_event = get_event(eventID);
    read_unlock_irqrestore(&eventID_list_lock, flags);
    /* Read unlocked. */
    


    /* If event not found. */
    if (this_event == NULL) {
        printk("error sys_doeventsig(): event not found. eventID = %d\n", eventID);
        return -1;
    }



    /* Check accessibility. */
    uid_t uid = current->cred->euid;
    gid_t gid = current->cred->egid;
    if (uid != 0 && (uid != this_event->UID || this_event->UIDFlag == 0) && (gid != this_event->GID || this_event->GIDFlag == 0)) {
        printk("sys_doeventsig(): access denied\n");
        return -1;
    }


    /* Lock wait queue. */
    spin_lock_irqsave(&(this_event->wait_queue.lock), flags);
    /* Get the number of processes waiting on this event. */
    int processes_signaled = get_list_length(&(this_event->wait_queue.task_list));
    /* Unlock wait queue. */
    spin_unlock_irqrestore(&(this_event->wait_queue.lock), flags);
    
    /* Wake up tasks in the wait queue. */
    wake_up(&(this_event->wait_queue));
    

    return processes_signaled;
}




/*
 * If eventIDs != NULL && num >= event_count, copy all event IDs to user array pointed to by eventIDs.
 * If eventIDs == NULL do not copy.
 * Return the number of active events on success.
 * Return -1 if num is less than the number of events or on failure.
 */
asmlinkage long sys_doeventinfo(int num, int * eventIDs)
{
    /* Remember to check if event is initialized at kernel boot before actually doing anything. */
    if (event_initialized == false) {
        printk("error sys_doeventinfo(): event not initialized\n");
        return -1;
    }


    unsigned long flags;
    /* Lock read. */
    read_lock_irqsave(&eventID_list_lock, flags);
    /* Count events. */
    int event_count = get_list_length(&global_event.eventID_list);

    /* Kmalloc an array for storing event IDs. */
    int * sys_eventIDs;
    if ((sys_eventIDs = kmalloc(event_count * sizeof(int), GFP_KERNEL)) == NULL) {
        printk("error sys_doeventinfo(): kmalloc()\n");
        return -1;
    }

    /* Insert all event IDs to array pointed to by sys_eventIDs. */
    int i = 0;
    struct event * pos; 
     
    list_for_each_entry(pos, &global_event.eventID_list, eventID_list) {    
            
        /* Do not insert or count global_event whose eventID is 0. */
        if ((pos->eventID) != 0) {  
            *(sys_eventIDs + i++) = pos->eventID;   
        }
    }

    read_unlock_irqrestore(&eventID_list_lock, flags);
    /* Read unlocked. */

    
    
        
    /* Check arguments. */
    if (eventIDs == NULL) {
        kfree(sys_eventIDs);
        return event_count;
    }

    if (num < event_count) {
        kfree(sys_eventIDs);
        return -1;

    }

    
    
    

    /* Copy to user. */
    if (copy_to_user(eventIDs, sys_eventIDs, event_count * sizeof(int)) != 0) {
        printk("error sys_doeventinfo(): copy_to_user()\n");
        return -1;
    }

    kfree(sys_eventIDs);
    
    return event_count;
}





/*
 * Change the UID and GID of the event with the given eventID to the given UID and GID.
 * Return 0 on success.
 * Return -1 on failure.
 * Access denied:
 *  uid != 0 && uid != event->UID
 */
asmlinkage long sys_doeventchown(int eventID, uid_t UID, gid_t GID)
{
    /* Remember to check if event is initialized at kernel boot before actually doing anything. */
    if (event_initialized == false) {
        printk("error sys_doeventchown(): event not initialized\n");
        return -1;
    }


    unsigned long flags;
    /* Lock Read. */
    read_lock_irqsave(&eventID_list_lock, flags);
     /* Search for the event in event list. */
    struct event * this_event = get_event(eventID);
    read_unlock_irqrestore(&eventID_list_lock, flags);
    /* Read unlocked. */
    
    /* If event not found. */
    if (this_event == NULL) {
        printk("error sys_doeventchown(): event not found. eventID = %d\n", eventID);
        return -1;
    }


    /* Check accessibility. */
    uid_t uid = current->cred->euid;
    if (uid != 0 && uid != this_event->UID) {
        printk("sys_doeventchown(): access denied\n");
        return -1;
    }



    this_event->UID = UID;
    this_event->GID = GID;

    return 0;
}





/*
 * Change event->UIDFlag to UIDFlag and event->GIDFlag to GIDFlag.
 * Return 0 on success.
 * Retutn -1 on failure.
 * Access denied:
 *  uid != 0 && uid != event->UID
 */
asmlinkage long sys_doeventchmod(int eventID, int UIDFlag, int GIDFlag)
{
    /* Remember to check if event is initialized at kernel boot before actually doing anything. */
    if (event_initialized == false) {
        printk("error sys_doeventchmod(): event not initialized\n");
        return -1;
    }

    /* Check arguments. */
    if (UIDFlag < 0 || UIDFlag > 1 || GIDFlag < 0 || GIDFlag > 1) {
        printk("error sys_doeventchmod(): invalid arguments\n");
        return -1;
    }

    unsigned long flags;   
    /* Lock read. */    
    read_lock_irqsave(&eventID_list_lock, flags);
    /* Search for the event in event list. */    
    struct event * this_event = get_event(eventID);
    read_unlock_irqrestore(&eventID_list_lock, flags);
    /* Read unlocked. */

    /* If event not found. */
    if (this_event == NULL) {
        printk("error sys_doeventchmod(): event not found. eventID = %d\n", eventID);
        return -1;
    }

    /* Check accessibility. */
    uid_t uid = current->cred->euid;
    if (uid != 0 && uid != this_event->UID) {
        printk("sys_doeventchmod(): access denied\n");
        return -1;
    }

    this_event->UIDFlag = UIDFlag;
    this_event->GIDFlag = GIDFlag;

    return 0;
}





/*
 * Place the UID, GID, User Signal Enable Bit and Group Signal Enable Bit into the memory pointed to by UID, GID, UIDFlag and GIDFlag respectively.
 * Return 0 on success.
 * Return -1 on filure.
 */
asmlinkage long sys_doeventstat(int eventID, uid_t * UID, gid_t * GID, int * UIDFlag, int * GIDFlag)
{
    /* Remember to check if event is initialized at kernel boot before actually doing anything. */
    if (event_initialized == false) {
        printk("error sys_doeventstat(): event not initialized\n");
        return -1;
    }

    /* Check arguments. */
    if (UID == NULL || GID == NULL || UIDFlag == NULL || GIDFlag == NULL) {
        printk("error sys_doeventstat(): invalid eventID\n");
        return -1;
    }

    unsigned long flags;  
    /* Lock read. */
    read_lock_irqsave(&eventID_list_lock, flags); 
    /* Search for the event in event list. */
    struct event * this_event = get_event(eventID); 
    read_unlock_irqrestore(&eventID_list_lock, flags); 
    /* Read unlocked. */

    /* If event not found. */
    if (this_event == NULL) {
        printk("error sys_doeventstat(): event not found. eventID = %d\n", eventID);
        return -1;
    }

    


    if (copy_to_user(UID, &(this_event->UID), sizeof(uid_t)) != 0) {
        printk("error sys_doeventstat(): copy_to_user()\n");
        return -1;
    }

    if (copy_to_user(GID, &(this_event->GID), sizeof(gid_t)) != 0) {
        printk("error sys_doeventstat(): copy_to_user()\n");
        return -1;
    }

    if (copy_to_user(UIDFlag, &(this_event->UIDFlag), sizeof(int)) != 0) {
        printk("error sys_doeventstat(): copy_to_user()\n");
        return -1;
    }

    if (copy_to_user(GIDFlag, &(this_event->GIDFlag), sizeof(int)) != 0) {
        printk("error sys_doeventstat(): copy_to_user()\n");
        return -1;
    }


    return 0;
}

