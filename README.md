# My daemon templates for Linux


## Description

A **daemon** is a type of program on Unix-like operating systems that runs unobtrusively in the background, 
rather than under the direct control of a user, waiting to be activated by the occurance of a specific event or condition.


**The standard procedure for daemonize a process has a few steps:**

  * **Fork**, allowing the parent process to terminate.
  * Set the umask to zero (reset umask).
  * Open any logs for writing (optional).
  * Start a new session for the daemon by calling **setsid()**.
  * Change the current working directory to a safe location (default: **"/"**).
  * Redirect **stdin**, **stdout** and **stderr** to /dev/null (not redirect for DEBUG mode).


***
<br/>
## Daemon templates for Linux

##### Template 1 (uses  daemon() function from unistd.h) [template_1](./template_1/)

**Advantages:**

1. Very simple.


**Disadvantages:**

1. DEBUG mode not support.
2. Don't create a PID file.


See man-pages for **daemon()** function: [man 3 daemon()](http://man7.org/linux/man-pages/man3/daemon.3.html)

Implementing function **daemon()** in uClibc: [daemon.c](http://git.uclibc.org/uClibc/tree/libc/unistd/daemon.c)


***
<br/>
## License

[BSD](./LICENSE).
