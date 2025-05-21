#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "history.h"

#define HISTORY_FILENAME ".history.txt"

static HistoryList *hist_head = NULL;
static HistoryList *hist_tail = NULL;
static HistoryList *hist_curr = NULL;

void addHistory(const char *cmd) {
    HistoryList *node = malloc(sizeof(HistoryList));
    node->command = strdup(cmd);
    node->hlist_next = NULL;
    node->hlist_previous = hist_tail;

    if(hist_tail) {
        hist_tail->hlist_next = node;
    } else {
        hist_head = node;
    }

    hist_tail = node;
}

void save_history(const char *command) {
    FILE *file = fopen(HISTORY_FILENAME,"a");
    if(!file) return;
    
    fprintf(file,"%s\n",command);
    fclose(file);
}

void load_history() {
    FILE *file = fopen(HISTORY_FILENAME,"r");
    if(!file) return;
    char line[1024];
    while(fgets(line, sizeof(line), file)) {
        line[strcspn(line,"\n")] = '\0';
        addHistory(line);
    }
    fclose(file);
}
 
HistoryList *history_get_tail() { return hist_tail; }
HistoryList *history_get_head() { return hist_head; }
HistoryList *history_get_current() { return hist_curr; }
void history_set_current(HistoryList *node) { hist_curr = node; }