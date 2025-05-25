#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "raw_input.h"
#include "history.h"
#include "autocomplete.h"

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
    int cursor = 0;
    char c;
    bool history_flag = false;
    
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
            } else if(c == 127) {
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
                            printf("\033[34m%s\033[0m", buffer);
                            fflush(stdout);
                            history_flag = true;
                        }
                    }
                    else if (seq[1] == 'D') {
                        if (position > 0) {
                            position--;
                            printf("\033[D");
                            fflush(stdout);
                        }
                    }else if (seq[1] == 'C') { 
                        if (position < strlen(buffer)) {
                            position++;
                            printf("\033[C");
                            fflush(stdout);
                        }
                    }
                }
            }  else {
                repaintHelper(&history_flag, position, buffer);
                printf("\033[33m%c\033[0m", c);
                fflush(stdout);
                buffer[position++] = c;
                buffer[position] = '\0';
                char* autocomplete_suggestion = autocomplete(TRIE_ROOT, buffer);
                if(autocomplete_suggestion) {
                    printf("reached here");
                    printf("%s",autocomplete_suggestion);
                    free(autocomplete_suggestion);
                }

                if (position >= buffsize) {
                    buffsize += 1024;
                    buffer = realloc(buffer, buffsize);
                    if (!buffer) {
                        fprintf(stderr, "Allocation error\n");
                        exit(EXIT_FAILURE);
                    }
                }
            }
        }
    }
    buffer[position] = '\0';
    return buffer;
}

void repaintHelper(bool *repaint_required, int position, char *buffer) {
    if(*repaint_required) {
        for(int i = 0; i < position; i++) {
            printf("\b \b");
        }
        printf("\033[33m%s\033[0m", buffer);
        fflush(stdout);
        *repaint_required = false;
    }
}