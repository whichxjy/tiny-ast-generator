#include "parser.h"
#include "tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// source code file
FILE *src_file;
// result file
FILE *result_file;

// index of current source code line
int line_idx = 0;

// check if there's any syntax error
int SYNTAX_ERROR = FALSE;

int main(int argc, char **argv) {
    // source code file name
    char filename[256];
    if (argc != 2) {
        fprintf(stderr, "usage: %s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    strcpy(filename, argv[1]);

    // open source code file
    src_file = fopen(filename, "r");
    if (src_file == NULL) {
        fprintf(stderr, "File %s not found\n", filename);
        exit(EXIT_FAILURE);
    }

    // write result to stdout
    result_file = stdout;

    // create ast
    TreeNode *ast = parse();
    if (!SYNTAX_ERROR) {
        fprintf(result_file, "[========== AST ==========]\n");
        print_tree(ast);
    }

    // free ast
    free_tree(ast);
    // close file
    fclose(src_file);
}