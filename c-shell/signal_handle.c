#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "signal_handler.h"

void setup_signal_handlers() {
    struct sigaction sa_int, sa_tstp, sa_chld, sa_term;
    
    memset(&sa_int, 0, sizeof(sa_int));
    sa_int.sa_handler = handle_sigint;
    sa_int.sa_flags = SA_RESTART;
    sigemptyset(&sa_int.sa_mask);
    sigaction(SIGINT, &sa_int, NULL);

    memset(&sa_tstp, 0, sizeof(sa_tstp));
    sa_tstp.sa_handler = handle_sigtstp;
    sa_tstp.sa_flags = SA_RESTART;
    sigemptyset(&sa_int.sa_mask);
    sigaction(SIGTSTP, &sa_tstp, NULL);

    memset(&sa_chld, 0, sizeof(sa_chld));
    sa_chld.sa_handler = handle_sigchld;
    sa_chld.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigemptyset(&sa_chld.sa_mask);
    sigaction(SIGCHLD, &sa_chld, NULL);

    memset(&sa_term, 0, sizeof(sa_term));
    sa_term.sa_handler = handle_sigterm;
    sigemptyset(&sa_term.sa_mask);
    sigaction(SIGTERM, &sa_term, NULL);
}

//2
void handle_sigint(int sig) {
    const char *msg = "\nSIGINT Received returning to Boss process\n";
    write(STDOUT_FILENO, msg, strlen(msg));
}

//15 Custom:CTRL+T
void handle_sigterm(int sig) {
    const char *msg = "\nSIGTERM received, exiting shell...\n";
    write(STDOUT_FILENO, msg, strlen(msg));
    _exit(0);  
}

//18
void handle_sigtstp(int sig) {
    //noop
}

//20
void handle_sigchld(int sig) {
    while (waitpid(-1, NULL, WNOHANG) > 0) {
    }
    printf("\nAll child processes successfully reaped\n");
}

