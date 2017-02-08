#ifndef DAEMON_HEADER
#define DAEMON_HEADER





extern int daemonized;



struct daemon_info_t
{
    int no_chdir;
    int no_close_stdio;

    const char *pid_file;
    const char *log_file;
    const char *cmd_pipe;
};


extern struct daemon_info_t daemon_info;





int redirect_stdio_to_devnull(void);
int create_pid_file(const char *pid_file_name);



void daemon_error_exit(const char *format, ...);
void exit_if_not_daemonized(int exit_status);



void daemonize(void);





#endif //DAEMON_HEADER
