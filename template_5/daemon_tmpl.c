#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <getopt.h>
#include <limits.h>
#include <pthread.h>


#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>


#include "daemon.h"





static const char *help_str =
        " Daemon name:  " DAEMON_NAME          "\n"
        " Daemon  ver:  " DAEMON_VERSION_STR   "\n\n"
#ifdef  DEBUG
        " Build  mode:  debug\n"
#else
        " Build  mode:  release\n"
#endif
        " Build  date:  " __DATE__ "\n"
        " Build  time:  " __TIME__ "\n"
#if  CHANGED_FILES == 0
        " Build  hash:  "  COMMIT_HASH "\n\n"
#else
        " Build  hash:  *" COMMIT_HASH "\n\n"
#endif
        "Options:                      description:\n\n"
        "       --no_chdir             Don't change the directory to '/'\n"
        "       --no_fork              Don't do fork\n"
        "       --no_close             Don't close standart IO files\n"
        "       --pid_file [value]     Set pid file name\n"
        "       --log_file [value]     Set log file name\n"
        "       --cmd_pipe [value]     Set CMD Pipe name\n"
        "  -v,  --version              Display daemon version\n"
        "  -h,  --help                 Display this help\n\n";



// indexes for long_opt function
enum
{
    cmd_opt_help    = 'h',
    cmd_opt_version = 'v',

    //daemon options
    cmd_opt_no_chdir,
    cmd_opt_no_fork,
    cmd_opt_no_close,
    cmd_opt_pid_file,
    cmd_opt_log_file,
    cmd_opt_cmd_pipe
};



static const char *short_opts = "hv";


static const struct option long_opts[] =
{
    { "version",      no_argument,       NULL, cmd_opt_version  },
    { "help",         no_argument,       NULL, cmd_opt_help     },

    //daemon options
    { "no_chdir",     no_argument,       NULL, cmd_opt_no_chdir },
    { "no_fork",      no_argument,       NULL, cmd_opt_no_fork  },
    { "no_close",     no_argument,       NULL, cmd_opt_no_close },
    { "pid_file",     required_argument, NULL, cmd_opt_pid_file },
    { "log_file",     required_argument, NULL, cmd_opt_log_file },
    { "cmd_pipe",     required_argument, NULL, cmd_opt_cmd_pipe },

    { NULL,           no_argument,       NULL,  0 }
};



pthread_t  pthread_cmd_pipe;




void daemon_exit_handler(int sig)
{

    //Here we release resources


    unlink(daemon_info.pid_file);
    unlink(daemon_info.cmd_pipe);

    _exit(EXIT_FAILURE);
}



void init_signals(void)
{
    set_sig_handler(SIGINT,  daemon_exit_handler); //for Ctrl-C in terminal for debug (in debug mode)
    set_sig_handler(SIGTERM, daemon_exit_handler);

    set_sig_handler(SIGCHLD, SIG_IGN); // ignore child
    set_sig_handler(SIGTSTP, SIG_IGN); // ignore tty signals
    set_sig_handler(SIGTTOU, SIG_IGN);
    set_sig_handler(SIGTTIN, SIG_IGN);
    set_sig_handler(SIGHUP,  SIG_IGN);
}



void processing_cmd(int argc, char *argv[])
{
    int opt;

    // We use the processing_cmd function for processing the command line and
    // for commands from the DAEMON_CMD_PIPE_NAME
    // For this we use the getopt_long function several times
    // to work properly, we must reset the optind
    optind = 0;


    while( (opt = getopt_long(argc, argv, short_opts, long_opts, NULL)) != -1 )
    {
        switch( opt )
        {

            case cmd_opt_help:
                        puts(help_str);
                        exit_if_not_daemonized(EXIT_SUCCESS);
                        break;

            case cmd_opt_version:
                        puts(DAEMON_NAME "  version  " DAEMON_VERSION_STR "\n");
                        exit_if_not_daemonized(EXIT_SUCCESS);
                        break;


                 //daemon options
            case cmd_opt_no_chdir:
                        daemon_info.no_chdir = 1;
                        break;

            case cmd_opt_no_fork:
                        daemon_info.no_fork = 1;
                        break;

            case cmd_opt_no_close:
                        daemon_info.no_close_stdio = 1;
                        break;

            case cmd_opt_pid_file:
                        daemon_info.pid_file = optarg;
                        break;

            case cmd_opt_log_file:
                        daemon_info.log_file = optarg;
                        break;

            case cmd_opt_cmd_pipe:
                        daemon_info.cmd_pipe = optarg;
                        break;

            default:
                        puts("for more detail see help\n\n");
                        exit_if_not_daemonized(EXIT_FAILURE);
                        break;
        }
    }
}



void* cmd_pipe_thread(void *thread_arg)
{
    int   fd;
    int   argc;
    char *arg;
    char **argv;
    char *cmd_pipe_buf;



    pthread_detach(pthread_self());
    unlink(daemon_info.cmd_pipe);


    argv = (char **)malloc(PIPE_BUF*sizeof(char *));
    if( !argv )
        daemon_error_exit("Can't get mem for argv (CMD_PIPE): %m\n");


    cmd_pipe_buf = (char *)malloc(PIPE_BUF);
    if( !cmd_pipe_buf )
        daemon_error_exit("Can't get mem for cmd_pipe_buf: %m\n");


    if( mkfifo(daemon_info.cmd_pipe, 0622) != 0 )
        daemon_error_exit("Can't create CMD_PIPE: %m\n");


    fd = open(daemon_info.cmd_pipe, O_RDWR);
    if( fd == -1 )
        daemon_error_exit("Can't open CMD_PIPE: %m\n");



    while(1)
    {
        memset(cmd_pipe_buf, 0, PIPE_BUF);


        if( read(fd, cmd_pipe_buf, PIPE_BUF) == -1 )             // wait for command from DAEMON_CMD_PIPE_NAME
            daemon_error_exit("read CMD_PIPE return -1: %m\n");


        argc = 1;                                                // see getopt_long function
        arg  = strtok(cmd_pipe_buf, " \t\n");


        while( (arg != NULL)  && (argc < PIPE_BUF) )
        {
            argv[argc++] = arg;
            arg          = strtok(NULL, " \t\n");
        }


        if( argc > 1 )
          processing_cmd(argc, argv);
    }


    return NULL;
}



void init(void *data)
{
    init_signals();


    if( pthread_create(&pthread_cmd_pipe, NULL, cmd_pipe_thread, NULL) != 0 )
       daemon_error_exit("Can't create thread_cmd_pipe: %m\n");


    //Here is your code to initialize
}



int main(int argc, char *argv[])
{
    processing_cmd(argc, argv);
    daemonize2(init, NULL);


    while( !daemon_info.terminated )
    {

        // Here а routine of daemon

        printf("%s: daemon is run\n", DAEMON_NAME);
        sleep(10);
    }


    return EXIT_SUCCESS; // good job (we interrupted (finished) main loop)
}
