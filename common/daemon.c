#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>


#include "daemon.h"





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
 *  We uses our flag daemonized in daemon_info_t
 */



struct daemon_info_t daemon_info =
{
    .terminated = 0,
    .daemonized = 0,                   //flag will be set in finale function daemonize()

    #ifdef  DAEMON_NO_CHDIR
        .no_chdir = DAEMON_NO_CHDIR,
    #else
        .no_chdir = 0,
    #endif


    #ifdef  DAEMON_NO_CLOSE_STDIO
        .no_close_stdio = DAEMON_NO_CLOSE_STDIO,
    #else
        .no_close_stdio = 0,
    #endif


    #ifdef  DAEMON_PID_FILE_NAME
        .pid_file = DAEMON_PID_FILE_NAME,
    #else
        .pid_file = NULL,
    #endif


    #ifdef  DAEMON_LOG_FILE_NAME
        .log_file = DAEMON_LOG_FILE_NAME,
    #else
        .log_file = NULL,
    #endif


    #ifdef  DAEMON_CMD_PIPE_NAME
        .cmd_pipe = DAEMON_CMD_PIPE_NAME,
    #else
        .cmd_pipe = NULL,
    #endif
};





void exit_if_not_daemonized(int exit_status)
{
    if( !daemon_info.daemonized )
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



void daemonize2(void (*optional_init)(void *), void *data)
{
    switch( fork() )                                     // Become background process
    {
        case -1:  daemon_error_exit("Can't fork: %m\n");
        case  0:  break;                                 // child process (go next)
        default:  _exit(EXIT_SUCCESS);                   // We can exit the parent process
    }


    // ---- At this point we are executing as the child process ----



    // Reset the file mode mask
    umask(0);



    // Create a new process group(session) (SID) for the child process
    if( setsid() == -1 )
        daemon_error_exit("Can't setsid: %m\n");



    // Change the current working directory to "/"
    // This prevents the current directory from locked
    // The demon must always change the directory to "/"
    if( !daemon_info.no_chdir && (chdir("/") != 0) )
        daemon_error_exit("Can't chdir: %m\n");



    if( daemon_info.pid_file && (create_pid_file(daemon_info.pid_file) == -1) )
        daemon_error_exit("Can't create pid file: %s: %m\n", daemon_info.pid_file);



    // call user functions for the optional initialization
    // before closing the standardIO (STDIN, STDOUT, STDERR)
    if( optional_init )
        optional_init(data);



    if( !daemon_info.no_close_stdio && (redirect_stdio_to_devnull() != 0) )
        daemon_error_exit("Can't redirect stdio to /dev/null: %m\n");



    daemon_info.daemonized = 1; //good job
}
