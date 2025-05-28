#ifndef LOGICAL_H
#define LOGICAL_H

typedef enum {
    NODE_CMD,
    NODE_AND,
    NODE_OR
}NodeType;

typedef struct _AST_Node {
    NodeType node;
    char** argv;
    struct _AST_Node* left;
    struct _AST_Node* right;
} ASTNode;

ASTNode* parse_chain(char* cmds);
int eval_ast(ASTNode* node);
void free_ast(ASTNode* node);

#endif