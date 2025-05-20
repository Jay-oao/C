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

#define TOK_DELIM " \t\r\n"
#define RED "\x1b[31m"
#define YELLOW "\x1b[33m"
#define RESET "\x1b[0m"  


char * *tokenize(char * );
int xec_call(char ** );

int main () {

    char *LINE;
    char **ARGS;
    int status = 1;

    do{
        printf(RED "<0Sk> " RESET);
        fflush(stdout);
        LINE = getLine();
        addHistory(LINE);
        history_set_current(NULL);
        ARGS = tokenize(LINE);
        xec_call(ARGS);
        free(LINE);
        free(ARGS);
    } while(status);

    return 0;
}

char * *tokenize(char *line) {

    int buffsize = 1024;
    char * *tokens = malloc(sizeof(char *)*buffsize);
    int position = 0;

    if(!tokens) {
        fprintf(stderr, "SEGMENTATION FAULT\n");
        exit(EXIT_FAILURE);
    }

    char *token = strtok(line, TOK_DELIM);
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
        token = strtok(NULL, TOK_DELIM);
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
