
Haoyuan Xiao
Ran Pang



**********     Files Modified or Added to The Kernel Source Code    ********

	Suppose the working path is now the linux kernel main folder.

	1. In “./arch/x86/include/asm/unistd_64.h”,
	   comment “#define __NR_getpmsg” and “__SYSCALL(__NR_getpmsg, sys_ni_syscall)”, 
	   add line “#define __NR_prinfo 181” and “__SYSCALL(__NR_prinfo, prinfo)”.

	2. In “./include/linux/“ add file “prinfo.h”.

	3. In “./kernel/” add file “prinfo.c”,
	   add line “obj-y += prinfo.o” in “Makefile”.



************     Job Distribution      ***********

	1. Haoyuan Xiao worked on
	   modifying “unistd_64.h” and “Makefile”,
	   fetching the “youngest_child_pid”, “younger_sibling_pid”,
	   “older_sibling_pid”, “start_time”, “user_time”, “sys_time”,
	   “cutime”, “cstime”, “uid”, “comm[16]”.

	2. Ran Pang worked on
	   writing “prinfo.h”, building structure of “prinfo.c”,
	   fetching “state”, “pid”, “parent_pid”, “signal”,
	   writing “test.c”,
	   “expa.c”, “expb.c”, “expc.c” for the experiment.



*************	  Experiment Result    ************

	1. 
	    state: 1
   	    pid: 238
   	    parent pid: 52
   	    youngest child pid: -1
   	    younger sibling pid: 239
   	    older sibling pid: -1
   	    start time 276
   	    user time: 2
   	    system time: 4
   	    children user time: 0
   	    children system time: 0
   	    uid: 0
   	    comm: expa.o
   	    signal: 0

	2.
	    state: 0
   	    pid: 241
   	    parent pid: 52
   	    youngest child pid: -1
   	    younger sibling pid: -1
   	    older sibling pid: -1
   	    start time 391
   	    user time: 8
   	    system time: 22
   	    children user time: 0
   	    children system time: 0
   	    uid: 0
   	    comm: expb.o
   	    signal: 0

	    state: 0
   	    pid: 242
   	    parent pid: 52
   	    youngest child pid: -1
   	    younger sibling pid: -1
   	    older sibling pid: -1
   	    start time 474
   	    user time: 2
   	    system time: 7
   	    children user time: 0
   	    children system time: 0
   	    uid: 0
   	    comm: expb.o
   	    signal: 0

	    state: 0
   	    pid: 243
   	    parent pid: 52
   	    youngest child pid: -1
   	    younger sibling pid: -1
   	    older sibling pid: -1
   	    start time 1243
   	    user time: 0
   	    system time: 7
   	    children user time: 0
   	    children system time: 0
   	    uid: 0
   	    comm: expb.o
   	    signal: 0
	
	Explanations:
	    >state represent the current state of the process. each state is assigned an integer. state = 0 means process is currently running.
	    >available pids are probably stored in a queue initialized in increasing order. 
	    >parent pid = 52 means all processes running under shell are children of process shell.
	    >youngest child pid = -1, younger sibling pid = -1, older sibling pid = -1 means this process has no child process nor sibling process.
	    >start time = 1243 means the process start 1243 seconds after the kernel boots.
	    >user time = 2 means 2 units of cpu time are spent in user-mode code within the process.
	    >system time = 7 means 7 units of cpu time are spent in the kernel within the process.
	    >children user time is the sum of user time of all children processes.
	    >children system time is the sum of system time of all children processes.
	    >uid is the id of user who create the process. uid = 0 means the process is created by root.
	    >comm is the first argument in the main function of the process.
	    >signal is a set of consecutive bits that records the pending signals in the process. each signal type owns a bit which is set to 1 if this type of signal is pending or 0 the other way around. signal = 0 means there is no pending signals.

	3. signal = 2048
	   The reason why signals cannot be queued lies in the way linux system stores pending signals.
	   Linux system uses an array of long to store pending signals.
	   Each type of signal occupies a certain bit in the array.
	   If a signal is pending, the corresponding bit is set to 1.
	   Otherwise it is set to 0.
	   Thus the binary method cannot represent more than one instances.
	   Therefore linux system cannot queue signals of the same type.
	    