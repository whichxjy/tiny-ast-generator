#include "scanner.h"
#include <string.h>
#include <ctype.h>

// states in scanner DFA
typedef enum {
    START,
    IN_COMMENT,
    IN_ASSIGN,
    IN_ID,
    IN_INTEGER,
    IN_FLOAT,
    DONE
} StateType;

// lexeme of each token, including id and reserved word
char lexeme[MAX_TOKEN_SIZE + 1];

// max line size
#define LINE_BUF_SIZE 256

// buffer of current line
static char line_buf[LINE_BUF_SIZE];
// current position in line buffer
static int line_pos = 0;
// current line buffer size
static int buf_size = 0;
// end of file flag
static int EOF_flag = FALSE;

// get the next character in the line
static char get_next_char(void) {
    if (line_pos < buf_size) {
        return line_buf[line_pos++];
    }
    else {
        // go to the next line
        line_idx += 1;
        // try to read the next line
        if (fgets(line_buf, LINE_BUF_SIZE, src_file)) {
            buf_size = strlen(line_buf);
            line_pos = 0;
            return line_buf[line_pos++];
        }
        else {
            // end of file
            EOF_flag = TRUE;
            return EOF;
        }
    }
}

// cancel the result of get_next_char
static void cancel_current_char(void) {
    if (!EOF_flag) {
        line_pos -= 1;
    }
}

// reserved word
typedef struct {
    char *name;
    TokenType token_type;
} ReservedWord;

// reserved words
ReservedWord reserved_words[RESERVED_WORD_NUM] = {
    { "read", READ_TOKEN },
    { "write", WRITE_TOKEN },
    { "if", IF_TOKEN },
    { "then", THEN_TOKEN },
    { "else", ELSE_TOKEN },
    { "end", END_TOKEN },
    { "repeat", REPEAT_TOKEN },
    { "until", UNTIL_TOKEN },
    { "break", BREAK_TOKEN },
    { "continue", CONTINUE_TOKEN },
    { "proc", PROC_TOKEN },
    { "begin", BEGIN_TOKEN },
    { "call", CALL_TOKEN }
};

// check if it's a reserved word
static TokenType reserved_look_up(char *str) {
    for (int i = 0; i < RESERVED_WORD_NUM; i++) {
        if (!strcmp(str, reserved_words[i].name)) {
            return reserved_words[i].token_type;
        }
    }
    return ID_TOKEN;
}

// get the next token in source file
TokenType get_next_token(void) {
    // index in lexeme
    int lexeme_idx = 0;
    // type of current token
    TokenType current_token;
    // current DFA state
    StateType current_dfa_state = START;
    // save current char to lexeme or not
    int save_current_char;
    // DFA
    while (current_dfa_state != DONE) {
        char current_char = get_next_char();
        save_current_char = TRUE;
        // go to the next state
        switch(current_dfa_state) {
            // ============= [Start] =============
            case START:
                if (current_char == ' ' || current_char == '\t' || current_char == '\n') {
                    save_current_char = FALSE;
                }
                else if (current_char == '{') {
                    current_dfa_state = IN_COMMENT;
                    save_current_char = FALSE;
                }
                else if (current_char == ':') {
                    current_dfa_state = IN_ASSIGN;
                }
                else if (isalpha(current_char)) {
                    current_dfa_state = IN_ID;
                }
                else if (isdigit(current_char)) {
                    current_dfa_state = IN_INTEGER;
                }
                else {
                    current_dfa_state = DONE;
                    // deal with current token
                    switch (current_char) {
                        case '=':
                            current_token = EQ_TOKEN;
                            break;
                        case '<':
                            current_token = LT_TOKEN;
                            break;
                        case '+':
                            current_token = ADD_TOKEN;
                            break;
                        case '-':
                            current_token = SUB_TOKEN;
                            break;
                        case '*':
                            current_token = MUL_TOKEN;
                            break;
                        case '/':
                            current_token = DIV_TOKEN;
                            break;
                        case '(':
                            current_token = LPAREN_TOKEN;
                            break;
                        case ')':
                            current_token = RPAREN_TOKEN;
                            break;
                        case ';':
                            current_token = SEMI_TOKEN;
                            break;
                        case EOF:
                            current_token = ENDFILE_TOKEN;
                            save_current_char = FALSE;
                            break;
                        default:
                            current_token = ERROR_TOKEN;
                            break;
                    }
                }
                break;
            // ============= [In Comment] =============
            case IN_COMMENT:
                save_current_char = FALSE;
                if (current_char == '}') {
                    current_dfa_state = START;
                }
                else if (current_char == EOF) {
                    current_token = ENDFILE_TOKEN;
                    current_dfa_state = DONE;
                }
                break;
            // ============= [In Assign] =============
            case IN_ASSIGN:
                // check if the next token is ':='
                if (current_char == '=') {
                    current_token = ASSIGN_TOKEN;
                }
                else {
                    // error
                    current_token = ERROR_TOKEN;
                    cancel_current_char();
                    save_current_char = FALSE;
                }
                current_dfa_state = DONE;
                break;
            // ============= [In ID] =============
            case IN_ID:
                if (!isalnum(current_char)) {
                    // finish scanning id
                    current_token = ID_TOKEN;
                    cancel_current_char();
                    save_current_char = FALSE;
                    current_dfa_state = DONE;
                }
                break;
            // ============= [In Integer] =============
            case IN_INTEGER:
                if (current_char == '.') {
                    current_dfa_state = IN_FLOAT;
                }
                else if (!isdigit(current_char)) {
                    // finish scanning number
                    current_token = INTEGER_TOKEN;
                    cancel_current_char();
                    save_current_char = FALSE;
                    current_dfa_state = DONE;
                }
            // ============= [In Float] =============
            case IN_FLOAT:
                if (!isdigit(current_char)) {
                    // finish scanning number
                    current_token = FLOAT_TOKEN;
                    cancel_current_char();
                    save_current_char = FALSE;
                    current_dfa_state = DONE;
                }
            // ============= [Done] =============
            case DONE:
                break;
            // ============= [Error] =============
            default:
                // should never happen
                current_token = ERROR_TOKEN;
                current_dfa_state = DONE;
                break;
        }
        // save current char?
        if (save_current_char && lexeme_idx <= MAX_TOKEN_SIZE) {
            lexeme[lexeme_idx] = current_char;
            lexeme_idx += 1;
        }
        // done?
        if (current_dfa_state == DONE) {
            lexeme[lexeme_idx] = '\0';
            // check if id is a reserved word
            if (current_token == ID_TOKEN) {
                current_token = reserved_look_up(lexeme);
            }  
        }
    }
    return current_token;
}
