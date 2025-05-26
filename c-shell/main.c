#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>
#include <stdbool.h>
#include "tokenize.h"
#include "raw_input.h"
#include "history.h"
#include "redirect.h"
#include "autocomplete.h"

#define TOK_DELIM " \t\r\n"
#define RED "\x1b[31m"
#define RESET "\x1b[0m"
#define PIPE_DELIM "|"  

// TODO: Handle edge cases more gracefully
// TODO: Better signal Handling
// TODO: Support conditional Chaining 

char * *tokenize(char* , char* );
int xec_call(char** );
void exec_pipe_call(char** );

TrieNode* TRIE_ROOT = NULL;

int main () {

    char *LINE;
    char **ARGS;
    int status = 1;
    TRIE_ROOT = create_node();
    if (!TRIE_ROOT) {
        fprintf(stderr, "Failed to allocate memory for TRIE_ROOT\n");
        exit(EXIT_FAILURE);
    }
    
    load_history();
    load_context();

    do{
        printf(RED "<0Sk> " RESET);
        fflush(stdout);
        LINE = getLine();
        addHistory(LINE);
        save_history(LINE);
        history_set_current(NULL);
        if(strchr(LINE, '|') != NULL) {
            ARGS = tokenize(LINE,PIPE_DELIM);
            exec_pipe_call(ARGS);   
        } else {
            ARGS = tokenize(LINE, TOK_DELIM);
            xec_call(ARGS);
        }
        save_context(LINE);
        free(LINE);
        free(ARGS);
    } while(status);

    return 0;
}