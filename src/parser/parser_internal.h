#ifndef PARSER_INTERNAL_H
#define PARSER_INTERNAL_H

#include "../ast/ast.h"
#include "../lexer/lexer.h"
#include "../lexer/token/token.h"

/*
    Internal parser structures and functions
    Not exposed to the user
*/

// Parser structure holding the lexer and current token
struct parser
{
    struct lexer lexer;
    struct token *current_token;
};

// Return the type of the current token without consuming it
enum token_type peek(struct parser *parser);

// Consume the current token and advance to the next one
void pop(struct parser *parser);

/*
    Remove a separator (semicolon or newline) if present
    Returns 1 if a separator was removed, 0 otherwise
*/
int remove_separator(struct parser *p);

//  Pop tokens until a non-newline token is found
void skip_newlines(struct parser *p);

// Pop tokens until a non-newline and non-semicolon token is found
void skip_semicolon_newline(struct parser *p);

/*
    Parse a simple command
    Grammar: simple_command = { assignment_word | redirection } WORD { element }
    Returns the AST node for the simple command or NULL on error
*/
struct ast *parse_simple_command(struct parser *p);

/*
    Parse a list of commands separated by semicolons or newlines
    Grammar: list = and_or { separator and_or }
*/
struct ast *parse_list(struct parser *p);

/*
    Parse a single command
    Grammar: command = simple_command | if_command | while_command | for_command
*/
struct ast *parse_command(struct parser *p);

/*
    Parse a compound list
    Grammar: compound_list = { \n } and_or { ( ';' | '\n' ) { '\n' } and_or } [ ';' ] { '\n' }
    Stop when one of the given end tokens is encountered
*/
struct ast *parse_compound_list(struct parser *p, const enum token_type *end_token,
                           size_t end_token_count);

/*
    Parse an if statement
    Grammar: if_command = 'if' compound_list 'then' compound_list
                          [ 'elif' compound_list 'then' compound_list ]*
                          [ 'else' compound_list ] 'fi'
*/                           
struct ast *parse_if(struct parser *p);

/*
    Parse a while statement
    Grammar: while_command = 'while' compound_list 'do' compound_list 'done'
*/
struct ast *parse_while(struct parser *p);

/*
    Parse an until statement
    Grammar: until_command = 'until' compound_list 'do' compound_list 'done'
*/
struct ast *parse_until(struct parser *p);

/*
    Parse a for statement
    Grammar: for_command = 'for' WORD 'in' WORD { WORD } 'do' compound_list 'done'
*/
struct ast *parse_for(struct parser *p);

/*
    Parse a pipeline
    Grammar: pipeline = [ '!' ] command { '|' { '\n' } command }
*/
struct ast *parse_pipeline(struct parser *p);

/*
    Parse logical AND/OR operations
    Grammar: and_or = pipeline { ( '&&' | '||' ) { '\n' } pipeline }
*/
struct ast *parse_and_or(struct parser *p);

/*
    Parse a redirection
    Grammar: redirection = [ IO_NUMBER ] redir_op WORD
*/
struct ast *parse_redirection(struct parser *p);

/*
    Check if a token type is a redirection operator
*/
int is_redirection_token(enum token_type type);

/*
    Parse an assignment
    Grammar: NAME = VALUE
*/
struct ast *parse_assignment(struct parser *p);

/*
    Parse a block enclosed in braces
    Grammar: block = '{' compound_list '}'
*/
struct ast *parse_block(struct parser *p);

#endif /* ! PARSER_INTERNAL_H */
