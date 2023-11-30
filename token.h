// The tokenizer should handle five types of tokens : TOK_WORD, TOK_QUOTED_WORD, TOK_LESSTHAN, TOK_GREATERTHAN, and TOK_PIPE
// TOK_WORD: a sequence of non-whitespace characters
// TOK_QUOTED_WORD: a sequence of characters enclosed in double quotes
// TOK_LESSTHAN: the < character (input redirection)
// TOK_GREATERTHAN: the > character (output redirection)
// TOK_PIPE: the | character (pipe)

#ifndef TOKEN_H
#define TOKEN_H

typedef enum
{
    TOK_WORD,
    TOK_QUOTED_WORD,
    TOK_LESSTHAN,
    TOK_GREATERTHAN,
    TOK_PIPE,
    TOK_END
} TokenType;

typedef struct
{
    TokenType type;
    char *text;
} Token;

#endif