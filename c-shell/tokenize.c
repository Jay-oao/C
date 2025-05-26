#include <stdio.h>
#include <stdlib.h>

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