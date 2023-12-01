/*
 * tokenize.c
 *
 * Functions to tokenize and manipulate lists of tokens
 *
 * Author: Howdy Pierce <howdy@sleepymoose.net>
 * Contributor: Niyomwungeri Parmenide Ishimwe <parmenin@andrew.cmu.edu>
 */
#include <stdio.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "clist.h"
#include "tokenize.h"
#include "token.h"

// Documented in .h file
const char *TT_to_str(TokenType tt)
{
    switch (tt)
    {
    case TOK_WORD:
        return "WORD";
    case TOK_QUOTED_WORD:
        return "QUOTED_WORD";
    case TOK_LESSTHAN:
        return "LESSTHAN";
    case TOK_GREATERTHAN:
        return "GREATERTHAN";
    case TOK_PIPE:
        return "PIPE";
    case TOK_END:
        return "(end)";
    }
    __builtin_unreachable();
}

// Documented in .h file
CList TOK_tokenize_input(const char *input, char *errmsg, size_t errmsg_sz)
{
    CList tokens = CL_new();

    while (input != NULL && *input != '\0')
    {
        if (isspace(*input))
            input++;

        else if (*input)
        {
            // if (*input == '"')
            // {
            //     const char *endQuoted = input + 1; // Skip the opening quote
            //     while (*endQuoted != '"')
            //     {
            //         endQuoted++;
            //         if (*endQuoted == '\0')
            //         {
            //             snprintf(errmsg, errmsg_sz, "Position %d: unmatched quote", (int)(input - errmsg));
            //             CL_free(tokens);
            //             return NULL;
            //         }
            //     }

            //     // Allocate a new token
            //     Token tok = {TOK_QUOTED_WORD, strndup(input + 1, endQuoted - input - 1)};

            //     // append the token to the list of tokens
            //     CL_append(tokens, (Token)tok);

            //     // Advance the input pointer
            //     input = endQuoted + 1;
            // }

            // Find the end of the word
            char *word = malloc(strlen(input) + 1);
            const char *end = input;

            while (*end != '\0' && !isspace(*end) && *end != '<' && *end != '>' && *end != '|')
            {
                // if escape character
                if (*end == '\\')
                {
                    if (*end == '\0')
                    {
                        snprintf(errmsg, errmsg_sz, "Position %d: unmatched escape character", (int)(input - errmsg));
                        CL_free(tokens);
                        return NULL;
                    }
                    end += 2;
                }

                end++;
                printf("end: %c\n", *end);
            }

            // Allocate a new token
            snprintf(word, end - input + 1, "%s", input);
            Token tok = {TOK_WORD, strdup(word)};

            // append the token to the list of tokens
            CL_append(tokens, (Token)tok);

            // Advance the input pointer
            input = end;

            // Free the word
            free(word);
        }
        // else if (*input == '<')
        // {
        //     Token tok = {TOK_LESSTHAN, NULL};
        //     CL_append(tokens, (Token)tok);
        //     input++;
        // }
        // else if (*input == '>')
        // {
        //     Token tok = {TOK_GREATERTHAN, NULL};
        //     CL_append(tokens, (Token)tok);
        //     input++;
        // }
        // else if (*input == '|')
        // {
        //     Token tok = {TOK_PIPE, NULL};
        //     CL_append(tokens, (Token)tok);
        //     input++;
        // }
        // else
        // {
        //     Token tok = {TOK_END, NULL};
        //     CL_append(tokens, (Token)tok);
        //     snprintf(errmsg, errmsg_sz, "Position %d: unexpected character %c", (int)(input - errmsg), *input);
        //     CL_free(tokens);
        //     return NULL;
        // }
    }

    // Return the final list of tokens
    return tokens;
}

// Documented in .h file
TokenType TOK_next_type(CList tokens)
{
    if (tokens == NULL)
        return TOK_END;

    if (tokens->head != NULL)
        return tokens->head->tok_elt.type;

    return TOK_END;
}

// Documented in .h file
Token TOK_next(CList tokens)
{
    Token nextToken = {TOK_END, NULL};
    if (tokens == NULL || tokens->head == NULL)
        return nextToken;

    return tokens->head->tok_elt;
}

// Documented in .h file
void TOK_consume(CList tokens)
{
    if (tokens == NULL)
        return;

    if (tokens->head != NULL)
        CL_remove(tokens, 0);
}

// Documented in .h file
void print_element(int pos, Token tok, void *cb_data)
{
    Token *data = (Token *)cb_data;

    if (tok.type == TOK_END)
        printf("%s: %d %s\n", (char *)data, pos, TT_to_str(tok.type));
    else
        printf("%s: %d %s %s\n", (char *)data, pos, TT_to_str(tok.type), tok.text);
}

void TOK_print(CList tokens)
{
    if (tokens == NULL)
        return;

    // For debugging: Prints the list of tokens, one per line
    CL_foreach(tokens, print_element, "DEBUG OUTPUT");
}