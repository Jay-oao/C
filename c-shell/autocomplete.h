#ifndef AUTOCOMPLETE_H
#define AUTOCOMPLETE_H

#define ALPHABET_SIZE 128

typedef struct TrieNode {
    struct TrieNode* children[ALPHABET_SIZE];
    bool isEndOfWord;
}TrieNode;

extern TrieNode* TRIE_ROOT;

TrieNode* create_node();
void insert(TrieNode* root, const char* word);
bool search(TrieNode* root, const char* word);
char* autocomplete(TrieNode* root, const char* prefix);
void load_context();
void save_context(char* valid_cmd);

#endif