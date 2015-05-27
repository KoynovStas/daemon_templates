#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>




/*
 *  How can I execute a new process from GNOME Terminal
 *  so that the child process's parent PID becomes 1 and not
 *  the PID of the ubuntu session init process?
 *
 *  This is intentionally hard.
 *  Service managers want to keep track of orphaned child processes.
 *  They want not to lose them to process #1.
 *  Stop trying to do that!!!
 *
 *
 *  If you are asking solely because you think that your process (daemon)
 *  ought to have a parent process ID of 1, then wean yourself off this idea.
 *
 *
 *  Therefore, modern Linux system does not correctly determine the fact of
 *  daemonized through the function:
 *
 *  if( getppid() == 1 )
 *      return 1;        //already a daemon
 *
 */







int daemonized = 0;





void exit_if_not_daemonized(int exit_status)
{
    if( !daemonized )
        _exit(exit_status);
}



void daemon_error_exit(const char *format, ...)
{
    va_list ap;


    if( format &&  *format )
    {
        va_start(ap, format);
        fprintf(stderr, "%s: ", DAEMON_NAME);
        vfprintf(stderr, format, ap);
        va_end(ap);
    }


    _exit(EXIT_FAILURE);
}



int redirect_stdio_to_devnull(void)
{
    int fd;


    fd = open("/dev/null", O_RDWR);
    if(fd == -1)
        return -1; //error can't open file


    dup2(fd, STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);


    if(fd > 2)
        close(fd);


    return 0; //good job
}



int create_pid_file(const char *pid_file_name)
{
    int fd;
    const int BUF_SIZE = 32;
    char buf[BUF_SIZE];



    if( !pid_file_name )
    {
        errno = EINVAL;
        return -1;
    }


    fd = open(pid_file_name, O_RDWR | O_CREAT, 0644);
    if(fd == -1)
        return -1; // Could not create on PID file


    if( lockf(fd, F_TLOCK, 0) == -1 )
    {
        close(fd);
        return -1; // Could not get lock on PID file
    }


    if( ftruncate(fd, 0) != 0 )
    {
        close(fd);
        return -1; // Could not truncate on PID file
    }


    snprintf(buf, BUF_SIZE, "%ld\n", (long)getpid());
    if( write(fd, buf, strlen(buf)) != (int)strlen(buf) )
    {
        close(fd);
        return -1; // Could not write PID to PID file
    }


    return fd; //good job
}



void daemonize(void)
{

    pid_t pid;


    pid = fork();
    if( pid == -1 )
        daemon_error_exit("Can't fork: %m\n");



    // If we got a good PID, then we can exit the parent process
    if( pid > 0 )
        _exit(EXIT_SUCCESS);



    // ---- At this point we are executing as the child process ----



    // Reset the file mode mask
    umask(0);



    // Create a new process group (SID) for the child process
    if( setsid() == -1 )
        daemon_error_exit("Can't setsid: %m\n");



#if  ( DEBUG == 0) || ( DAEMON_NO_CHDIR == 0 )
    // Change the current working directory to "/"
    // This prevents the current directory from locked
    // The demon must always change the directory to "/"
    // But in DEBUG mode can be defined DAEMON_NO_CHDIR == 1
    // Then the director will not be changed !!!
    if( chdir("/") != 0 )
        daemon_error_exit("Can't chdir: %m\n");
#endif



#ifdef  DAEMON_PID_FILE_NAME
    if( create_pid_file(DAEMON_PID_FILE_NAME) == -1 )
        daemon_error_exit("Can't create pid file: %s: %m\n", DAEMON_PID_FILE_NAME);
#endif



#ifndef  DEBUG
    // Redirect standard files to /dev/null
    if( redirect_stdio_to_devnull() != 0 )
        daemon_error_exit("Can't redirect stdio to /dev/null: %m\n");
#endif



    daemonized = 1; //good job
}
