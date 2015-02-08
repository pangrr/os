OS Machine Problem 3: Eventcalls

Jingwei Xu
Ran Pang



**********      Files Modified or Added to The Kernel Source Code    **********


	Suppose the working path is now the linux kernel main folder.

	> Replace “./arch/x86/include/asm/unistd_64.h”.

	> In “./include/linux/“ add “eventcalls.h” and "prinfo.h".

	> In “./kernel/” add “eventcalls.c” and "prinfo.c".
	   
	> Replace “./kernel/Makefile”.

    > Replace "./init/main.c".



****************            Job Distribution                   ****************


	> Jingwei Xu built system call 205, 211, 214 and test program t5, t6, t7, 
    t8, t9.

	> Ran Pang built all system call 181, 182, 183, 184, 185 and test program 
    t1, t2, t3, t4.



*****************           About Test Programs                 ***************
	
    
    > Use the test programs collaboratively we can test all the
    functions of our system calls.
    
    > Test programs from t1.c to t5.c are created
    for basic test of the event system calls with full access control.
   
    > Test programs from t6.c to t9.c are added later 
    to accomplish the entire function test. 
    
    > state.c uses system call built in MP2
    to display the state of the task with given pid. 



****************         How to Use Test Programs                 *************
 	
	> ./state pid
	Use system call 299 (prinfo) to print
	the current state of the task with given pid.

	> ./t1
	Use system call 181 (doeventopen) to create event and
	print its eventID, then wait on it.

	> ./t2 eventID
	Print self's pid and use system call 183 (doeventwait)
	to wait for an event with given eventID.

	> ./t3 eventID
	Use system call 184 (doeventsig)
	to wake up all the processes waiting for the event with given eventID.

	> ./t4 eventID
	Using system call 182 (doeventclose)
	to wake up all the waiting processes,
	then close the event with given eventID.

	> ./t5 
	Call system call 185 (doeventinfo) twice:
	first get the number of events and create a pointer with allocated memory,
	then get the eventIDs.	

	> ./t6 UID GID UIDFlag GIDFlag
	Create an event with given UID, GID, UIDFlag, GIDFlag. 

	> ./t7 uid gid eventID system_call
	Create a task with given uid, gid
	can call given system call on event with given eventID.
    Effective system calls are:
    183 : sys_doeventwait
    184 : sys_doeventsig
    182 : sys_doeventclose

	> ./t8 uid eventID UID GID
	Create a task with given uid.
	Change the UID and GID of the event with given eventID
	to the given UID and GID.
	
	> ./t9 uid eventID UIDFlag GIDFlag
	Create a task with given uid.
	Change the UIDFlag and GIDFlag of the event with given eventID
	to the given UIDFlag and GIDFlag.


