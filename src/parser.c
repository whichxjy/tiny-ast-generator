#include "parser.h"
#include "scanner.h"
#include "tree.h"
#include "util.h"
#include <stdlib.h>

// current token
static TokenType current_token;

// functions
static TreeNode* program(void);
static TreeNode* proc_def(void);
static TreeNode* stmts(void);
static TreeNode* stmt(void);
static TreeNode* read_stmt(void);
static TreeNode* write_stmt(void);
static TreeNode* if_stmt(void);
static TreeNode* repeat_stmt(void);
static TreeNode* break_stmt(void);
static TreeNode* continue_stmt(void);
static TreeNode* assign_stmt(void);
static TreeNode* proc_call_stmt(void);
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

// main program
TreeNode* program(void) {
    CHECK_SYNTAX_ERROR
    // match proc-defs
    TreeNode *t = NULL;
    TreeNode *p = NULL;
    while (!SYNTAX_ERROR && current_token != ENDFILE_TOKEN
           && current_token == PROC_TOKEN) {
        TreeNode *q = proc_def();
        if (q != NULL) {
            if (t == NULL) {
                t = q;
                p = t;
            }
            else {
                p->sibling = q;
                p = q;
            }
        }
    }
    // match stmts
    p->sibling = stmts();
    return t;
}

TreeNode* proc_def(void) {
    CHECK_SYNTAX_ERROR
    TreeNode *t = new_proc_node();
    if (t != NULL) {
        // match "proc"
        match(PROC_TOKEN);
        // match procedure name
        TreeNode *p = new_expr_node(ID_EXPR);
        if (p != NULL && current_token == ID_TOKEN) {
            p->attr.name = copy_string(lexeme);
        }
        t->child[0] = p;
        match(ID_TOKEN);
        // match "begin"
        match(BEGIN_TOKEN);
        // match stmts
        t->child[1] = stmts();
        // match "end"
        match(END_TOKEN);
    }
    return t;
}

// statements
TreeNode* stmts(void) {
    CHECK_SYNTAX_ERROR
    // match one statement first
    TreeNode *t = NULL;
    TreeNode *p = NULL;
    while (!SYNTAX_ERROR && current_token != ENDFILE_TOKEN && current_token != END_TOKEN
           && current_token != ELSE_TOKEN && current_token != UNTIL_TOKEN) {
        match(SEMI_TOKEN);
        TreeNode *q = stmt();
        if (q != NULL) {
            if (t == NULL) {
                t = q;
                p = t;
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
        case READ_TOKEN:
            t = read_stmt();
            break;
        case WRITE_TOKEN:
            t = write_stmt();
            break;
        case IF_TOKEN:
            t = if_stmt();
            break;
        case REPEAT_TOKEN:
            t = repeat_stmt();
            break;
        case BREAK_TOKEN:
            t = break_stmt();
            break;
        case CONTINUE_TOKEN:
            t = continue_stmt();
            break;
        case ID_TOKEN:
            t = assign_stmt();
            break;
        case CALL_TOKEN:
            t = proc_call_stmt();
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

// break statement
TreeNode* break_stmt(void) {
    CHECK_SYNTAX_ERROR
    TreeNode *t = new_stmt_node(BREAK_STMT);
    if (t != NULL) {
        match(BREAK_TOKEN);
    }
    return t;
}

// continue statement
TreeNode* continue_stmt(void) {
    CHECK_SYNTAX_ERROR
    TreeNode *t = new_stmt_node(CONTINUE_STMT);
    if (t != NULL) {
        match(CONTINUE_TOKEN);
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

TreeNode* proc_call_stmt(void) {
    CHECK_SYNTAX_ERROR
    TreeNode *t = new_stmt_node(PROC_CALL_STMT);
    if (t != NULL) {
        match(CALL_TOKEN);
        if (current_token == ID_TOKEN) {
            t->attr.name = copy_string(lexeme);
        }
        match(ID_TOKEN);
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
            if (t != NULL) {
                t->attr.name = copy_string(lexeme);
            }
            match(ID_TOKEN);
            break;
        case INTEGER_TOKEN:
            t = new_expr_node(INTEGER_EXPR);
            if (t != NULL) {
                t->attr.integer_val = atoi(lexeme);
            }
            match(INTEGER_TOKEN);
            break;
        case FLOAT_TOKEN:
            t = new_expr_node(FLOAT_EXPR);
            if (t != NULL) {
                t->attr.float_val = atof(lexeme);
            }
            match(FLOAT_TOKEN);
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
    TreeNode *t = program();
    if (!SYNTAX_ERROR && current_token != ENDFILE_TOKEN) {
        // syntax error
        print_syntax_error("Code ends before file!\n");
    }
    return t;
}