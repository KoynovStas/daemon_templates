#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>





int main(void)
{

    if( daemon(DAEMON_NO_CHDIR, DAEMON_NO_CLOSE_STDIO) != 0 )
        return EXIT_FAILURE;



    while(1)
    {

        // Here а routine of daemon

        printf("%s: daemon is run\n", DAEMON_NAME);
        sleep(10);
    }



    return EXIT_SUCCESS; // good job (we interrupted (finished) main loop)
}
