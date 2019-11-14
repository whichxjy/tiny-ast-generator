#include "parser.h"
#include "scanner.h"
#include "tree.h"
#include "util.h"
#include <stdlib.h>

// current token
static TokenType current_token;

// functions
static TreeNode* stmts(void);
static TreeNode* stmt(void);
static TreeNode* if_stmt(void);
static TreeNode* repeat_stmt(void);
static TreeNode* assign_stmt(void);
static TreeNode* read_stmt(void);
static TreeNode* write_stmt(void);
static TreeNode* expr(void);
static TreeNode* simple_expr(void);
static TreeNode* term(void);
static TreeNode* factor(void);

// print syntax error message
static void print_syntax_error(char *message) {
    SYNTAX_ERROR = TRUE;
    fprintf(result_file, "Syntax error at line %d: %s", line_idx, message);
}

// check syntax error
#define CHECK_SYNTAX_ERROR \
    if (SYNTAX_ERROR) {    \
        return NULL;       \
    }

// match the expected token
static void match(TokenType expected) {
    if (SYNTAX_ERROR) {
        return;
    }
    else if (current_token == expected) {
        current_token = get_next_token();
    }
    else {
        // syntax error
        SYNTAX_ERROR = TRUE;
        print_syntax_error("Unexpected Token -> ");
        print_token(current_token, lexeme);
    }
}

// statements
TreeNode* stmts(void) {
    CHECK_SYNTAX_ERROR
    // match one statement first
    TreeNode *t = stmt();
    TreeNode *p = t;
    while (!SYNTAX_ERROR && current_token != ENDFILE_TOKEN && current_token != END_TOKEN
           && current_token != ELSE_TOKEN && current_token != UNTIL_TOKEN) {
        match(SEMI_TOKEN);
        TreeNode *q = stmt();
        if (q != NULL) {
            if (t == NULL) {
                p = q;
                t = p;
            }
            else {
                p->sibling = q;
                p = q;
            }
        }
    }
    return t;
}

// statement
TreeNode* stmt(void) {
    CHECK_SYNTAX_ERROR
    TreeNode *t = NULL;
    switch (current_token) {
        case IF_TOKEN:
            t = if_stmt();
            break;
        case REPEAT_TOKEN:
            t = repeat_stmt();
            break;
        case ID_TOKEN:
            t = assign_stmt();
            break;
        case READ_TOKEN:
            t = read_stmt();
            break;
        case WRITE_TOKEN:
            t = write_stmt();
            break;
        default:
            // syntax error
            print_syntax_error("Unexpected Token -> ");
            print_token(current_token, lexeme);
            current_token = get_next_token();
            break;
    }
    return t;
}

// if statement
static TreeNode* if_stmt(void) {
    CHECK_SYNTAX_ERROR
    TreeNode *t = new_stmt_node(IF_STMT);
    match(IF_TOKEN);
    if (t != NULL) {
        t->child[0] = expr();
    }
    match(THEN_TOKEN);
    if (t != NULL) {
        t->child[1] = stmts();
    }
    if (current_token == ELSE_TOKEN) {
        match(ELSE_TOKEN);
        if (t != NULL) {
            t->child[2] = stmts();
        }
    }
    match(END_TOKEN);
    return t;
}

// repeat statement
TreeNode* repeat_stmt(void) {
    CHECK_SYNTAX_ERROR
    TreeNode *t = new_stmt_node(REPEAT_STMT);
    match(REPEAT_TOKEN);
    if (t != NULL) {
        t->child[0] = stmts();
    }
    match(UNTIL_TOKEN);
    if (t != NULL) {
        t->child[1] = expr();
    }
    return t;
}

// assign statement
TreeNode* assign_stmt(void) {
    CHECK_SYNTAX_ERROR
    TreeNode *t = new_stmt_node(ASSIGN_STMT);
    if (t != NULL && current_token == ID_TOKEN) {
        t->attr.name = copy_string(lexeme);
    }
    match(ID_TOKEN);
    match(ASSIGN_TOKEN);
    if (t != NULL) {
        t->child[0] = expr();
    }
    return t;
}

// read statement
TreeNode* read_stmt(void) {
    CHECK_SYNTAX_ERROR
    TreeNode *t = new_stmt_node(READ_STMT);
    match(READ_TOKEN);
    if (t != NULL && current_token == ID_TOKEN) {
        t->attr.name = copy_string(lexeme);
    }
    match(ID_TOKEN);
    return t;
}

// write statement
TreeNode* write_stmt(void) {
    CHECK_SYNTAX_ERROR
    TreeNode *t = new_stmt_node(WRITE_STMT);
    match(WRITE_TOKEN);
    if (t != NULL) {
        t->child[0] = expr();
    }
    return t;
}

// expression
TreeNode* expr(void) {
    CHECK_SYNTAX_ERROR
    TreeNode *t = simple_expr();
    if (current_token == LT_TOKEN || current_token == EQ_TOKEN) {
        TreeNode *p = new_expr_node(OP_EXPR);
        if (p != NULL) {
            p->child[0] = t;
            p->attr.op = current_token;
            t = p;
            match(current_token);
            t->child[1] = simple_expr();
        }
    }
    return t;
}

// simple expression
TreeNode* simple_expr(void) {
    CHECK_SYNTAX_ERROR
    TreeNode *t = term();
    while (current_token == ADD_TOKEN || current_token == SUB_TOKEN) {
        TreeNode *p = new_expr_node(OP_EXPR);
        if (p != NULL) {
            p->child[0] = t;
            p->attr.op = current_token;
            t = p;
            match(current_token);
            t->child[1] = term();
        }
    }
    return t;
}

// term
TreeNode* term(void) {
    CHECK_SYNTAX_ERROR
    TreeNode* t = factor();
    while (current_token == MUL_TOKEN || current_token == DIV_TOKEN) {
        TreeNode *p = new_expr_node(OP_EXPR);
        if (p != NULL) {
            p->child[0] = t;
            p->attr.op = current_token;
            t = p;
            match(current_token);
            p->child[1] = factor();
        }
    }
    return t;
}

// factor
TreeNode* factor(void) {
    CHECK_SYNTAX_ERROR
    TreeNode* t = NULL;
    switch (current_token) {
        case LPAREN_TOKEN:
            match(LPAREN_TOKEN);
            t = expr();
            match(RPAREN_TOKEN);
            break;
        case ID_TOKEN:
            t = new_expr_node(ID_EXPR);
            if (t != NULL && current_token == ID_TOKEN) {
                t->attr.name = copy_string(lexeme);
            }
            match(ID_TOKEN);
            break;
        case NUMBER_TOKEN:
            t = new_expr_node(CONST_EXPR);
            if (t != NULL && current_token == NUMBER_TOKEN) {
                t->attr.val = atoi(lexeme);
            }
            match(NUMBER_TOKEN);
            break;
        default:
            // syntax error
            print_syntax_error("Unexpected Token -> ");
            print_token(current_token, lexeme);
            current_token = get_next_token();
            break;
    }
    return t;
}

// parse and return a new syntax tree
TreeNode* parse(void) {
    current_token = get_next_token();
    TreeNode *t = stmts();
    if (!SYNTAX_ERROR && current_token != ENDFILE_TOKEN) {
        // syntax error
        print_syntax_error("Code ends before file!\n");
    }
    return t;
}