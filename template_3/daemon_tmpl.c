#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>


#include "daemon.h"





void daemon_exit_handler(int sig)
{

    //Here we release resources


    unlink(daemon_info.pid_file);

    _exit(EXIT_FAILURE);
}



void init_signals(void)
{
    set_sig_handler(SIGTERM, daemon_exit_handler);

    set_sig_handler(SIGCHLD, SIG_IGN); // ignore child
    set_sig_handler(SIGTSTP, SIG_IGN); // ignore tty signals
    set_sig_handler(SIGTTOU, SIG_IGN);
    set_sig_handler(SIGTTIN, SIG_IGN);
    set_sig_handler(SIGHUP,  SIG_IGN);
}



void init(void *data)
{
    init_signals();


    //Here is your code to initialize
}



int main(void)
{
    daemonize2(init, NULL);


    while( !daemon_info.terminated )
    {

        // Here а routine of daemon

        printf("%s: daemon is run\n", DAEMON_NAME);
        sleep(10);
    }


    return EXIT_SUCCESS; // good job (we interrupted (finished) main loop)
}
