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
#define RESERVED_WORD_NUM 13

// type of token
typedef enum {
    // reserved words
    READ_TOKEN,     // read
    WRITE_TOKEN,    // write
    IF_TOKEN,       // if
    THEN_TOKEN,     // then
    ELSE_TOKEN,     // else
    END_TOKEN,      // end
    REPEAT_TOKEN,   // repeat
    UNTIL_TOKEN,    // until
    BREAK_TOKEN,    // break
    CONTINUE_TOKEN, // continue
    PROC_TOKEN,     // proc
    BEGIN_TOKEN,    // begin
    CALL_TOKEN,     // call
    // [multi-character tokens]
    ID_TOKEN,       // id
    INTEGER_TOKEN,  // integer
    FLOAT_TOKEN,    // float
    // [special symbols]
    ASSIGN_TOKEN,   // :=
    EQ_TOKEN,       // =
    LT_TOKEN,       // <
    ADD_TOKEN,      // +
    SUB_TOKEN,      // -
    MUL_TOKEN,      // *
    DIV_TOKEN,      // /
    LPAREN_TOKEN,   // (
    RPAREN_TOKEN,   // )
    SEMI_TOKEN,     // ;
    // [book-keeping tokens]
    ENDFILE_TOKEN,  // end file
    ERROR_TOKEN     // error
} TokenType;

// type of parse tree node
typedef enum {
    // procedure node
    PROC_NODE,
    // statement node
    STMT_NODE,
    // expression node
    EXPR_NODE
} NodeType;

// type of statement
typedef enum {
    // read statement
    READ_STMT,
    // write statement
    WRITE_STMT,
    // if statement
    IF_STMT,
    // repeat statement
    REPEAT_STMT,
    // break statement
    BREAK_STMT,
    // continue statement
    CONTINUE_STMT,
    // assign statement
    ASSIGN_STMT,
    // procedure call statement
    PROC_CALL_STMT
} StmtType;

// type of expression
typedef enum {
    // id expression
    ID_EXPR,
    // integer expression
    INTEGER_EXPR,
    // float expression
    FLOAT_EXPR,
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
        // for id
        char *name;
        // for integer
        int integer_val;
        // for float
        float float_val;
        // for operator
        TokenType op;
    } attr;
} TreeNode;

#endif