#ifndef _TREE_H_
#define _TREE_H_

#include "global.h"

// create a procedure definition node
TreeNode* new_proc_node();
// create a statement node
TreeNode* new_stmt_node(StmtType stmt_type);
// create an expression node
TreeNode* new_expr_node(ExprType expr_type);

// free memory of the tree
void free_tree(TreeNode *t);

// print a token
void print_token(TokenType token_type, const char *lexeme);
// print a tree
void print_tree(TreeNode *t);

#endif