#ifndef TOKEN_H
#define TOKEN_H

typedef enum
{
    TOK_WORD,
    TOK_QUOTED_WORD,
    TOK_LESSTHAN,
    TOK_GREATERTHAN,
    TOK_PIPE
} TokenType;

typedef struct
{
    TokenType type;
    char *text;
} Token;

#endif