#include <stdio.h>
#include <stdlib.h>
#include "tokenize.h"

#define TOK_DELIM " \t\r\n"

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

ASTNode* ASTroot = NULL;

ASTNode* parse_chain(char* cmds) {
    char* p = cmds;
    NodeType op_type;
    char* op_pos = NULL;
   
    char* and_pos = strstr(cmds,"&&");
    char* or_pos = strstr(cmds,"||");

    if(and_pos && (!or_pos || and_pos < or_pos)) {
        op_type = NODE_AND;
        op_pos = and_pos;
    } 
    else if(or_pos) {
        op_type = NODE_OR;
        op_pos = or_pos;
    }

    if(op_pos == NULL){
        ASTNode* cmd_node = malloc(sizeof(ASTNode));
        cmd_node->node = NODE_CMD;
        cmd_node->argv = tokenize(cmds, TOK_DELIM);
        cmd_node->left = cmd_node->right = NULL;
        return cmd_node;
    }

    int left_len = op_pos - cmds;
    char* left_str = malloc(left_len + 1);
    strncpy(left_str, cmds, left_len);
    left_str[left_len] = '\0';

    char* right_str = strdup(op_pos + 2);

    ASTNode* op_node = malloc(sizeof(ASTNode));
    op_node->node = op_type;
    op_node->left = parse_chain(left_str);
    op_node->right = parse_chain(right_str); 

    free(left_str);
    free(right_str);

    return op_node;
}

int eval_ast(ASTNode* node) {
    if(!node) return -1;

    if(node->node == NODE_CMD) {
        return xec_call(node->argv);
    }

    int left_status = eval_ast(node->left);

    if(node->node == NODE_AND) {
        if(left_status == 0) {
            return eval_ast(node->right);
        } else {
            return left_status;
        }
    }

    if(node->node == NODE_OR) {
        if(left_status != 0 ) {
            return eval_ast(node->right);
        } else {
            return left_status;
        }
    }

    return -1;
}