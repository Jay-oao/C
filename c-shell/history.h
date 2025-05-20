#ifndef HISTORY_H
#define HISTORY_H

typedef struct _HISTORY_LIST {
    char *command;
    struct _HISTORY_LIST *hlist_next;
    struct _HISTORY_LIST *hlist_previous;
} HistoryList;

void addHistory(const char *);
HistoryList *history_get_tail();
HistoryList *history_get_head();
HistoryList *history_get_current();
void history_set_current(HistoryList *node);

#endif