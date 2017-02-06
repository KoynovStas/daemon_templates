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





#ifdef  DEBUG
        #define DAEMON_MODE_INFO   " Build  mode:  debug\n"
#else
        #define DAEMON_MODE_INFO   " Build  mode:  release\n"
#endif



static const char *short_opts = ":hv?";
static const char *help_str   = " ===============  Help  ===============\n"
                                " Daemon name:  %s\n"
                                " Daemon  ver:  %d.%d.%d\n"
                                DAEMON_MODE_INFO
                                " Build  time:  %s  %s\n\n"
                                "Options:                      description:\n\n"
                                "  -v   --version              Display daemon version information\n"
                                "  -h,  --help                 Display this information\n\n";



static const struct option long_opts[] = {
    { "version",      no_argument,       NULL, 'v' },
    { "help",         no_argument,       NULL, 'h' },
    { NULL,           no_argument,       NULL,  0  }
};



pthread_t  pthread_cmd_pipe;




void daemon_exit_handler(int sig)
{

    //Here we release resources


#ifdef  DAEMON_PID_FILE_NAME
    unlink(DAEMON_PID_FILE_NAME);
#endif


#ifdef  DAEMON_CMD_PIPE_NAME
    unlink(DAEMON_CMD_PIPE_NAME);
#endif


    _exit(EXIT_FAILURE);
}



void init_signals(void)
{

    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = daemon_exit_handler;
    if( sigaction(SIGTERM, &sa, NULL) != 0)
        daemon_error_exit("Can't set daemon_exit_handler: %m\n");



    signal(SIGCHLD, SIG_IGN); // ignore child
    signal(SIGTSTP, SIG_IGN); // ignore tty signals
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
}



void processing_cmd(int argc, char *argv[])
{

    int opt, long_index;


    // We use the processing_cmd function for processing the command line and
    // for commands from the DAEMON_CMD_PIPE_NAME
    // For this we use the getopt_long function several times
    // to work properly, we must reset the optind
    optind = 0;



    opt = getopt_long(argc, argv, short_opts, long_opts, &long_index);
    while( opt != -1 )
    {
        switch( opt )
        {


            case 'v':
                        printf("%s  version  %d.%d.%d\n", DAEMON_NAME, DAEMON_MAJOR_VERSION, DAEMON_MINOR_VERSION, DAEMON_PATCH_VERSION);
                        exit_if_not_daemonized(EXIT_SUCCESS);
                        break;

            case 'h':

                        printf(help_str, DAEMON_NAME, DAEMON_MAJOR_VERSION, DAEMON_MINOR_VERSION, DAEMON_PATCH_VERSION, __DATE__, __TIME__);
                        exit_if_not_daemonized(EXIT_SUCCESS);
                        break;
            case '?':
                        printf("Unsupported option: -%c see help\n", optopt);
                        exit_if_not_daemonized(EXIT_SUCCESS);
                        break;
            case ':':
                        printf("Option -%c requires an operand see help\n", optopt);
                        exit_if_not_daemonized(EXIT_SUCCESS);
                        break;


            case 0:     // long options


//                  if( strcmp( "name_options", long_opts[long_index].name ) == 0 )
//                  {
//                      //Processing of "name_options"
//                      break;
//                  }


            default:
                  break;
        }

        opt = getopt_long(argc, argv, short_opts, long_opts, &long_index);
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
    unlink(DAEMON_CMD_PIPE_NAME);


    argv = (char **)malloc(PIPE_BUF*sizeof(char *));
    if( !argv )
        daemon_error_exit("Can't get mem for argv (CMD_PIPE): %m\n");


    cmd_pipe_buf = (char *)malloc(PIPE_BUF);
    if( !cmd_pipe_buf )
        daemon_error_exit("Can't get mem for cmd_pipe_buf: %m\n");


    if( mkfifo(DAEMON_CMD_PIPE_NAME, 0622) != 0 )
        daemon_error_exit("Can't create CMD_PIPE: %m\n");


    fd = open(DAEMON_CMD_PIPE_NAME, O_RDWR);
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



void init_daemon(int argc, char *argv[])
{

    processing_cmd(argc, argv);
    daemonize();
    init_signals();


    if( pthread_create(&pthread_cmd_pipe, NULL, cmd_pipe_thread, NULL) != 0 )
       daemon_error_exit("Can't create thread_cmd_pipe: %m\n");
}



int main(int argc, char *argv[])
{

    init_daemon(argc, argv);



    while(1)
    {

        // Here а routine of daemon

        printf("%s: daemon is run\n", DAEMON_NAME);
        sleep(10);
    }



    return EXIT_SUCCESS; // good job (we interrupted (finished) main loop)
}
