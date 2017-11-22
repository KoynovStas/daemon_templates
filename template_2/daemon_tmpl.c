#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>


#include "daemon.h"





int main(void)
{
    daemonize();


    while( !daemon_info.terminated )
    {

        // Here а routine of daemon

        printf("%s: daemon is run\n", DAEMON_NAME);
        sleep(10);
    }


    return EXIT_SUCCESS; // good job (we interrupted (finished) main loop)
}
