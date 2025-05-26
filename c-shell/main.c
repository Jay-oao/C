#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>
#include <stdbool.h>
#include "raw_input.h"
#include "history.h"
#include "redirect.h"
#include "autocomplete.h"

#define TOK_DELIM " \t\r\n"
#define RED "\x1b[31m"
#define YELLOW "\x1b[33m"
#define RESET "\x1b[0m"
#define PIPE_DELIM "|"  

// TODO: Autocomplete maybe using tries?
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

char** tokenize(char* line, char* DELIM) {

    int buffsize = 1024;
    char * *tokens = malloc(sizeof(char *) * buffsize);
    int position = 0;

    if(!tokens) {
        fprintf(stderr, "SEGMENTATION FAULT\n");
        exit(EXIT_FAILURE);
    }

    char *token = strtok(line, DELIM);
    while(token != NULL) {
        tokens[position++] = token;
        if(position >= buffsize) {
            buffsize += 1024;
            tokens = realloc(tokens, sizeof(char *) * buffsize);

            if(!tokens) {
                fprintf(stderr, "SEGMENTATION FAULT\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

int xec_call(char * *args) {

    pid_t cpid;
    int status;
    int redirect_index = false;

    if(args[0] == NULL) {
        return 1;
    }

    if(strcmp(args[0],"exit") == 0){
        exit(EXIT_SUCCESS);
    }

    else if(strcmp(args[0],"cd") == 0){
        chdir(args[1]);
        return 1;
    }

    else if(strcmp(args[0],"pwd") == 0){
        if(args[1] != NULL ) {
            printf("Too many arguements\n\n");
            return 0;
        }
        char cwd[PATH_MAX];
        printf("%s\n",getcwd(cwd, PATH_MAX));
        return 1;
    }

    //Currently handles only one '>' '<' '>>'
    for(int i = 0 ; args[i] != NULL ; i++) {
        if(strcmp(args[i], ">") == 0 || strcmp(args[i], "<") == 0 || strcmp(args[i], ">>") == 0  ){
            redirect_index = i;
            break;
        }
    }

    if(redirect_index) {
        char *output_filename = args[redirect_index+1];
        redirect(redirect_index, output_filename, args);
        args[redirect_index] = NULL;
    }

    cpid = fork();

    if(cpid == 0) {
        if(execvp(args[0], args) < 0) {
            printf("Command not found \n");
            free(args);
            exit(EXIT_FAILURE);
        }
    } else if(cpid < 0) {
        printf("Error forking \n");
    } else {
        waitpid(cpid, &status, WUNTRACED);
        restore_streams();
    }

    return 1;
}

void exec_pipe_call(char **ARGS) {

    int num_of_pipes = -1;
    while(ARGS[num_of_pipes+1] != NULL) num_of_pipes++;

    int pipe_fds[2 * num_of_pipes];

    for(int i = 0 ; i< num_of_pipes; i++) {
        if(pipe(pipe_fds + (i*2)) < 0) {
            perror("Pipe creation failed");
            exit(EXIT_FAILURE);
        }
    }

    int pid;
    int curr_cmd_index = 0;

    while(ARGS[curr_cmd_index] != NULL) {
        char **command = tokenize(ARGS[curr_cmd_index], TOK_DELIM);
        pid = fork();
        if(pid == 0) {
            if(curr_cmd_index != 0) {
                dup2(pipe_fds[(curr_cmd_index-1) * 2], STDIN_FILENO);
            }

            if(curr_cmd_index != num_of_pipes) {
                dup2(pipe_fds[curr_cmd_index * 2 + 1], STDOUT_FILENO);
            }
            
            for(int i = 0 ; i < 2*num_of_pipes ; i++) {
                close(pipe_fds[i]);
            } 
            
            if (execvp(command[0], command) < 0) {
                perror("execvp");
                exit(EXIT_FAILURE);
            }
        }
        if(pid != 0) free(command);
        curr_cmd_index++;
    }

    for(int i = 0 ; i < 2*num_of_pipes ; i++) {
        close(pipe_fds[i]);
    }

    for(int i = 0 ; i <= num_of_pipes ; i++) {
        wait(NULL);
    }

}