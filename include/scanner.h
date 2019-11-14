#ifndef _SCANNER_H_
#define _SCANNER_H_

#include "global.h"

// max token size
#define MAX_TOKEN_SIZE 50

// lexeme of each token, including id and reserved word
extern char lexeme[MAX_TOKEN_SIZE + 1];

// get the next token in source file
TokenType get_next_token(void);

#endif