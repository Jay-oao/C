#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

void setup_signal_handlers();
void handle_sigint(int sig);
void handle_sigterm(int sig);
void handle_sigtstp(int sig);
void handle_sigchld(int sig);

#endif
