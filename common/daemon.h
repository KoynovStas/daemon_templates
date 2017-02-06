#ifndef DAEMON_HEADER
#define DAEMON_HEADER





extern int daemonized;



int redirect_stdio_to_devnull(void);
int create_pid_file(const char *pid_file_name);



void daemon_error_exit(const char *format, ...);
void exit_if_not_daemonized(int exit_status);
void daemonize(void);





#endif //DAEMON_HEADER
