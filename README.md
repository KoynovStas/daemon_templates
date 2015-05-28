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
##### Template 2 (use our daemonize() function) [template_2](./template_2/)

**Advantages:**

1. Full control (DEBUG mode).
2. Create a PID file.


**Disadvantages:**

1. Many code.



***
<br/>
##### Template 3 (use our daemonize() function) [template_3](./template_3/)

It's template 2 + function init_signals + daemon_exit_handler



***
<br/>
##### Template 4 (use our daemonize() function) [template_4](./template_4/)

It's template 3 + Processing the command line by using the function getopt_long



***
<br/>
##### Template 5 (use our daemonize() function) [template_5](./template_5/)

It's template 4 + DAEMON_CMD_PIPE (Management daemon via a control pipe using the function getopt_long)



<br/>
## Testing



##### Check that the daemon has no controlling terminal and is not a session leader

The controlling terminal and session ID of a process can be inspected using the ps command:

```console
ps -o ppid,pid,sid,tty,cmd -C name_daemon
```


This should output a table of the form:

```console
PPID   PID    SID    TT       CMD
2173   29964  29961  ?        ./name_daemon
```


#### Note:


>**Why has my process PPID is not 1?**
How can I execute a new process from GNOME Terminal so that the child process's parent PID becomes 1 and not the PID of the ubuntu session init process?

**Answers:**

>**This is intentionally hard**. Service managers want to keep track of orphaned child processes.
They want not to lose them to process #1. Stop trying to do that.
If you are asking solely because you think that your process ought to have a parent process ID of 1,
then wean yourself off this idea.
[see more](http://unix.stackexchange.com/questions/194182/orphan-processs-parent-id-is-not-1-when-parent-process-executed-from-gnome-term)


>There is no way to discern a regular process from a daemon process.
A daemon can be started from anywhere, not just init.
It is not necessary to double fork and detach from the terminal to be considered a daemon.
On GNOME systems, gnome-settings-daemon keeps its parent, logs to the parent's terminal,
and does not have PPID == 1 as its parent PID, yet it is still considered a daemon.
A daemon is simply a continuously running process.
They are, therefore, impossible to identify with one command.
[see more](http://unix.stackexchange.com/questions/159964/how-to-check-whether-a-process-is-daemon-or-not)



##### Check the current working directory

Obtain the working directory pathname from /proc:

```console
ls -l /proc/PID_DAEMON/cwd
```


The working directory of the process is the target of the softlink:

```console
lrwxrwxrwx 1 root root 0 Feb  6 15:57 cwd -> /
```



#####  Check file descriptors

The file descriptors of a running process can be inspected by looking in /proc:

```console
ls -l /proc/PID_DAEMON/fd
```


Each open descriptor is presented as a softlink. 
If a descriptor is associated with a filesystem object then the target of the softlink is the pathname of the object. 
For a daemon the output would typically be similar to:

```console
lrwx------ 1 root root 64 2011-02-08 06:40 0 -> /dev/null
lrwx------ 1 root root 64 2011-02-08 06:40 1 -> /dev/null
lrwx------ 1 root root 64 2011-02-08 06:40 2 -> /dev/null
```



***
<br/>
## Usage

**To start working, perform the following steps:**

* Choose the appropriate template.
* Fix DAEMON_XXX variables in the Makefile.
* Choose your compiler (or toolchain) in the Makefile (see variable $GCC).
* Add your code in main loop and your source files in the project.
* Achieve a clean build of the project (the complete absence of errors and warnings when you build the project).
* Perform testing demon.
* If you are using embedded Linux you can use the template start script [S90DaemonName](./S90DaemonName).



***
<br/>
## License

[BSD](./LICENSE).
