#include "tree.h"
#include <stdlib.h>

// create a procedure definition node
TreeNode* new_proc_node() {
    TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
    for (int i = 0; i < MAX_CHILDREN; i++) {
        t->child[i] = NULL;
    }
    t->sibling = NULL;
    t->node_type = PROC_NODE;
    t->line_idx = line_idx;
    return t;
}

// create a statement node
TreeNode* new_stmt_node(StmtType stmt_type) {
    TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
    for (int i = 0; i < MAX_CHILDREN; i++) {
        t->child[i] = NULL;
    }
    t->sibling = NULL;
    t->node_type = STMT_NODE;
    t->type.stmt_type = stmt_type;
    t->line_idx = line_idx;
    return t;
}

// create an expression node
TreeNode* new_expr_node(ExprType expr_type) {
    TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
    for (int i = 0; i < MAX_CHILDREN; i++) {
        t->child[i] = NULL;
    }
    t->sibling = NULL;
    t->node_type = EXPR_NODE;
    t->type.expr_type = expr_type;
    t->line_idx = line_idx;
    return t;
}

// free memory of the tree
void free_tree(TreeNode *t) {
    if (t != NULL) {
        // free sibling
        free_tree(t->sibling);
        // free subtrees
        for (int i = 0; i < MAX_CHILDREN; i++) {
            free_tree(t->child[i]);
        }
        // free this node
        free(t);
    }
}

// print a token
void print_token(TokenType token_type, const char *lexeme) {
    switch (token_type) {
        case READ_TOKEN:
        case WRITE_TOKEN:
        case IF_TOKEN:
        case THEN_TOKEN:
        case ELSE_TOKEN:
        case END_TOKEN:
        case REPEAT_TOKEN:
        case UNTIL_TOKEN:
        case BREAK_TOKEN:
        case CONTINUE_TOKEN:
        case PROC_TOKEN:
        case BEGIN_TOKEN:
        case CALL_TOKEN:
            fprintf(result_file, "Reserved Word: %s\n", lexeme);
            break;
        case ASSIGN_TOKEN:
            fprintf(result_file, ":=\n");
            break;
        case EQ_TOKEN:
            fprintf(result_file, "=\n");
            break;
        case LT_TOKEN:
            fprintf(result_file, "<\n");
            break;
        case ADD_TOKEN:
            fprintf(result_file, "+\n");
            break;
        case SUB_TOKEN:
            fprintf(result_file, "-\n");
            break;
        case MUL_TOKEN:
            fprintf(result_file, "*\n");
            break;
        case DIV_TOKEN:
            fprintf(result_file, "/\n");
            break;
        case LPAREN_TOKEN:
            fprintf(result_file, "(\n");
            break;
        case RPAREN_TOKEN:
            fprintf(result_file, ")\n");
            break;
        case SEMI_TOKEN:
            fprintf(result_file, ";\n");
            break;
        case ID_TOKEN:
            fprintf(result_file, "ID: %s\n", lexeme);
            break;
        case INTEGER_TOKEN:
            fprintf(result_file, "Integer: %s\n", lexeme);
            break;
        case FLOAT_TOKEN:
            fprintf(result_file, "Float: %s\n", lexeme);
            break;
        case ERROR_TOKEN:
            fprintf(result_file, "Error: %s\n", lexeme);
            break;
        default:
            fprintf(result_file, "Unknown Token: %s\n", lexeme);
            break;
    }
}

// the number of indent spaces
static int indent_space_num = 0;

// indent spaces
#define INDENT_SPACES 4
// increase indentation
#define INC_INDENT indent_space_num += INDENT_SPACES
// decrease indentation
#define DEC_INDENT indent_space_num -= INDENT_SPACES

// print spaces to indent
static void print_spaces() {
    for (int i = 0; i < indent_space_num; i++) {
        fprintf(result_file, " ");
    }
}

// print a tree
void print_tree(TreeNode *t) {
    INC_INDENT;
    while (t != NULL) {
        print_spaces();
        if (t->node_type == PROC_NODE) {
            fprintf(result_file, "Function Definition\n");
        }
        else if (t->node_type == STMT_NODE) {
            // print statement node
            switch (t->type.stmt_type) {
                case READ_STMT:
                    fprintf(result_file, "Read: %s\n", t->attr.name);
                    break;
                case WRITE_STMT:
                    fprintf(result_file, "Write\n");
                    break;
                case IF_STMT:
                    fprintf(result_file, "If\n");
                    break;
                case REPEAT_STMT:
                    fprintf(result_file, "Repeat\n");
                    break;
                case BREAK_STMT:
                    fprintf(result_file, "Break\n");
                    break;
                case CONTINUE_STMT:
                    fprintf(result_file, "Continue\n");
                    break;
                case ASSIGN_STMT:
                    fprintf(result_file, "Assign to: %s\n", t->attr.name);
                    break;
                case PROC_CALL_STMT:
                    fprintf(result_file, "Call Procedure: %s\n", t->attr.name);
                    break;
                default:
                    fprintf(result_file, "Unknown Statement Type\n");
                    break;
            }
        }
        else if (t->node_type == EXPR_NODE) {
            // print expression node
            switch (t->type.expr_type) {
                case ID_EXPR:
                    fprintf(result_file, "ID: %s\n", t->attr.name);
                    break;
                case INTEGER_EXPR:
                    fprintf(result_file, "Integer: %d\n", t->attr.integer_val);
                    break;
                case FLOAT_EXPR:
                    fprintf(result_file, "Float: %f\n", t->attr.float_val);
                    break;
                case OP_EXPR:
                    fprintf(result_file, "Op: ");
                    print_token(t->attr.op, NULL);
                    break;
                default:
                    fprintf(result_file, "Unknown Expression Type\n");
                    break;
            }
        }
        else {
            fprintf(result_file, "Unknown Node Type");
        }
        // print children
        for (int i = 0; i < MAX_CHILDREN; i++) {
            print_tree(t->child[i]);
        }
        // go to the next statement
        t = t->sibling;
    }
    DEC_INDENT;
}
