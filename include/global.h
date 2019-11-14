#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <stdio.h>

#define TRUE 1
#define FALSE 0

// source code file
extern FILE *src_file;
// result file
extern FILE *result_file;

// index of current source code line
extern int line_idx;

// check if there's any syntax error
extern int SYNTAX_ERROR;

// the number of reserved words
#define RESERVED_WORD_NUM 8

// type of token
typedef enum {
    // reserved words
    IF_TOKEN,      // if
    THEN_TOKEN,    // then
    ELSE_TOKEN,    // else
    END_TOKEN,     // end
    REPEAT_TOKEN,  // repeat
    UNTIL_TOKEN,   // until
    READ_TOKEN,    // read
    WRITE_TOKEN,   // write
    // [multi-character tokens]
    ID_TOKEN,      // id
    NUMBER_TOKEN,  // number
    // [special symbols]
    ASSIGN_TOKEN,  // :=
    EQ_TOKEN,      // =
    LT_TOKEN,      // <
    ADD_TOKEN,     // +
    SUB_TOKEN,     // -
    MUL_TOKEN,     // *
    DIV_TOKEN,     // /
    LPAREN_TOKEN,  // (
    RPAREN_TOKEN,  // )
    SEMI_TOKEN,    // ;
    // [book-keeping tokens]
    ENDFILE_TOKEN, // end file
    ERROR_TOKEN    // error
} TokenType;

// type of parse tree node
typedef enum {
    // statement node
    STMT_NODE,
    // expression node
    EXPR_NODE
} NodeType;

// type of statement
typedef enum {
    // if statement
    IF_STMT,
    // repeat statement
    REPEAT_STMT,
    // assign statement
    ASSIGN_STMT,
    // read statement
    READ_STMT,
    // write statement
    WRITE_STMT
} StmtType;

// type of expression
typedef enum {
    // id expression
    ID_EXPR,
    // number expression
    CONST_EXPR,
    // op expression
    OP_EXPR
} ExprType;

// max children node for parse tree node
#define MAX_CHILDREN 3

typedef struct treeNode {
    struct treeNode* child[MAX_CHILDREN];
    struct treeNode* sibling; // for statements
    int line_idx;
    NodeType node_type;
    // type of statement or expression
    union {
        StmtType stmt_type;
        ExprType expr_type;
    } type;
    // attribute
    union {
        // for operator
        TokenType op;
        // for number
        int val;
        // for id
        char *name;
    } attr;
} TreeNode;

#endif