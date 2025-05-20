#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "redirect.h"

void redirect(int , char * , char ** );

int saved_stdout = -1;
int saved_stdin = -1;

void redirect(int index, char * output_filename, char **args) {
    if(strcmp(args[index], ">") == 0) {
        saved_stdout = dup(STDOUT_FILENO);
        int outputfile_fd = open(output_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if(outputfile_fd < 0){
            perror("Error opening/creating file\n");
            exit(EXIT_FAILURE);
        }
        dup2(outputfile_fd, STDOUT_FILENO);
        close(outputfile_fd);
    }

    if(strcmp(args[index], ">>") == 0) {
        saved_stdout = dup(STDOUT_FILENO);
        int outputfile_fd = open(output_filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if(outputfile_fd < 0){
            perror("Error opening/creating file\n");
            exit(EXIT_FAILURE);
        }
        dup2(outputfile_fd, STDOUT_FILENO);
        close(outputfile_fd);
    }

    if(strcmp(args[index], "<") == 0) {
        saved_stdin = dup(STDIN_FILENO);
        int outputfile_fd = open(output_filename, O_RDONLY);
        if(outputfile_fd < 0) {
            perror("Error opening file or file does not exist\n");
            exit(EXIT_FAILURE);
        }
        dup2(outputfile_fd, STDIN_FILENO);
        close(outputfile_fd);
    }
} 

void restore_streams() {
    if(saved_stdout != -1) {
        dup2(saved_stdout, STDOUT_FILENO);
        close(saved_stdout);
        saved_stdout = -1;
    } else if(saved_stdin != -1) {
        dup2(saved_stdin, STDIN_FILENO);
        close(saved_stdin);
        saved_stdin = -1;
    }
}