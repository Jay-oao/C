#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include "raw_input.h"
#include "history.h"
#include "autocomplete.h"

#define GHOST "\x1b[90m"  
#define RESET "\x1b[0m"
#define CLEAN "\x1b[K"
#define MOV_LEFT "\033[D"
#define MOV_RIGHT "\033[C"
#define BLUE "\033[34m"
#define YELLOW "\033[33m"
#define T_RESET "\033[0m"
#define CTRLT 20
#define BACKSPACE 127
#define ARROWKEYSSTART '\033'

void repaintHelper(bool * , int , char *);

struct termios original;

void enableRawMode() {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    original = raw;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original);
}

char *getLine() {
    enableRawMode();
    atexit(disableRawMode);

    int buffsize = 1024;
    int position = 0;
    char c;
    bool history_flag = false;
    char* autocomplete_suggestion = NULL;
    
    char *buffer = malloc(sizeof(char)*buffsize);
    if(!buffer) {
        fprintf(stderr,"Allocation failed\n");
        exit(EXIT_FAILURE);
    }

    while(1){
        if(read(STDIN_FILENO, &c, 1) == 1){
            if(c == '\r' || c=='\n'){
                repaintHelper(&history_flag, position, buffer);
                printf("\n");
                break;
            } else if(c == BACKSPACE) {
                if(position > 0) {
                    position--;
                    printf("\b \b");
                    fflush(stdout);
                }
            } else if(c == '\033') {
                char seq[2];
                if(read(STDIN_FILENO, &seq[0], 1) == 0) continue;
                if(read(STDIN_FILENO, &seq[1],1) == 0) continue;
                
                if(seq[0] == '[') {
                    if(seq[1] == 'A') {
                        HistoryList *current = history_get_current();
                        if(current == NULL && history_get_tail() != NULL){
                            current = history_get_tail();
                        }
                        else if(current && current->hlist_previous != NULL){
                            current = current->hlist_previous;
                        }
                        
                        if(current) {
                            history_set_current(current);
                            for(int i = 0; i<position ; i++) {
                                printf("\b \b");
                            }
                            strcpy(buffer, current->command);
                            position = strlen(buffer);
                            printf("\033[34m%s\033[0m", buffer);
                            fflush(stdout);
                            history_flag = true;
                        }

                    }
                    else if(seq[1] == 'B') {
                        HistoryList *current = history_get_current();
                        if(current == NULL ) {
                            continue;
                        } 
                        else if(current && current->hlist_next != NULL) {
                            current = current->hlist_next;
                        } 

                        if(current) {
                            history_set_current(current);
                            for(int i = 0 ; i<position ; i++){
                                printf("\b \b");
                            }
                            strcpy(buffer, current->command);
                            position = strlen(buffer);
                            printf(BLUE "%s" T_RESET, buffer);
                            fflush(stdout);
                            history_flag = true;
                        }
                    }
                    else if (seq[1] == 'D') {
                        if (position > 0) {
                            position--;
                            printf(MOV_LEFT);
                            fflush(stdout);
                        }
                    }else if (seq[1] == 'C') {
                        if (autocomplete_suggestion && *autocomplete_suggestion != '\0') {
                            if(strlen(autocomplete_suggestion) > buffsize) {
                                buffsize += 1024;
                                buffer = realloc(buffer, buffsize);
                                if (!buffer) {
                                    fprintf(stderr, "Allocation error\n");
                                    exit(EXIT_FAILURE);
                                }
                            }
                            strcpy(buffer, autocomplete_suggestion);

                            for(int i = 0 ; i<position ; i++) printf("\b \b");
                            fflush(stdout);

                            position = strlen(buffer);
                            printf(YELLOW "%s" T_RESET, buffer);
                            fflush(stdout);

                            free(autocomplete_suggestion);
                            autocomplete_suggestion = NULL;
                        } 
                        else if (position < strlen(buffer)) {
                            position++;
                            printf(MOV_RIGHT);
                            fflush(stdout);
                        }
                    }
                }
            } else if(c == CTRLT) {
                kill(getpid(), SIGTERM);
            } else {
                repaintHelper(&history_flag, position, buffer);
                printf(YELLOW "%c" T_RESET, c);
                fflush(stdout);

                buffer[position++] = c;
                if (position >= buffsize) {
                    buffsize += 1024;
                    buffer = realloc(buffer, buffsize);
                    if (!buffer) {
                        fprintf(stderr, "Allocation error\n");
                        exit(EXIT_FAILURE);
                    }
                }

                buffer[position] = '\0';

                int suggestion_length;

                if (autocomplete_suggestion) {
                    free(autocomplete_suggestion);
                    autocomplete_suggestion = NULL;
                }

                autocomplete_suggestion = autocomplete(TRIE_ROOT, buffer);
                if (autocomplete_suggestion) {
                    suggestion_length = strlen(autocomplete_suggestion);

                    if (suggestion_length > position) {
                        const char *ghost_text = autocomplete_suggestion + position;
                        
                        if (suggestion_length + 1 > buffsize) {
                            buffsize = suggestion_length + 1024;
                            buffer = realloc(buffer, buffsize);
                            if (!buffer) {
                                fprintf(stderr, "Allocation error\n");
                                exit(EXIT_FAILURE);
                            }
                        }

                        printf(GHOST "%s" RESET, ghost_text);
                        fflush(stdout);
                        
                        for (int i = 0; i < strlen(ghost_text); i++) {
                            printf("\b");
                        }
                        fflush(stdout);
                        printf(CLEAN);
                    }
                }
            }
        }
    }
    buffer[position] = '\0';
    if (autocomplete_suggestion) {
        free(autocomplete_suggestion);
    }
    return buffer;
}

void repaintHelper(bool *repaint_required, int position, char *buffer) {
    if(*repaint_required) {
        for(int i = 0; i < position; i++) {
            printf("\b \b");
        }
        printf(YELLOW "%s" T_RESET, buffer);
        fflush(stdout);
        *repaint_required = false;
    }
}