#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "autocomplete.h"

#define ALPHABET_SIZE 128
#define CONTEXT_FILE ".context.txt"

TrieNode* create_node() {
    TrieNode* node = malloc(sizeof(TrieNode));
    if(node) {
        node->isEndOfWord = false;
        for(int i = 0 ; i < ALPHABET_SIZE ; i++) {
            node->children[i] = NULL;
        }
    }
    return node;
}

void insert(TrieNode* root, const char* word) {
    TrieNode *current = root;
    while(*word) {
        int index = (unsigned char)*word;
        if(!current->children[index]) {
            current->children[index] = create_node();
        }
        current = current->children[index];
        word++;
    }
    current->isEndOfWord = true;
}

bool search(TrieNode* root, const char* word) {
    TrieNode *current = root;
    while (*word) {
        unsigned char index = (unsigned char)*word;
        if (!current->children[index]) {
            return false;
        }
        current = current->children[index];
        word++;
    }
    return current->isEndOfWord;
}

char* autocomplete(TrieNode* root, const char* prefix) {
    TrieNode* current = root;
    char buffer[256];
    int depth = 0;
    while(*prefix) {
        int index = (unsigned char)*prefix;
        if(!current->children[index]){
            return NULL;
        }
        buffer[depth++] = *prefix++; 
        current = current->children[index];
    }

    while(!current->isEndOfWord) {
        int found = 0;
        for(int i = 0 ; i < ALPHABET_SIZE ; i++) {
            if(current->children[i]){
                buffer[depth++] = (char)i;
                current = current->children[i];
                found = 1;
                break;
            }
        }
        if(!found) {
            return NULL;
        }
    }
    buffer[depth] = '\0';
    // Why not return buffer itself? Invalidates itself and stack is cleared after exec.
    return strdup(buffer);
}

void load_context() {
    FILE* file = fopen(CONTEXT_FILE,"r");
    if(!file) return;
    char line[1024];
    while(fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        insert(TRIE_ROOT, line);
    }
    fclose(file);
}

void save_context(char* valid_cmd) {
    FILE* file = fopen(CONTEXT_FILE, "a");
    if(!file) return;
    fprintf(file,"%s\n",valid_cmd);
    fclose(file);
}