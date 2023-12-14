/*
 * tokenize.c
 *
 * Functions to tokenize and manipulate lists of tokens
 *
 * Author: Howdy Pierce <howdy@sleepymoose.net>
 * Contributor: Niyomwungeri Parmenide Ishimwe <parmenin@andrew.cmu.edu>
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "tokenize.h"

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
    }
    __builtin_unreachable();
}

// Documented in .h file
CList TOK_tokenize_input(const char *input, char *errmsg, size_t errmsg_sz)
{
    CList tokens = CL_new();
    // clear the error message
    errmsg[0] = '\0';

    while (input != NULL && *input != '\0')
    {
        if (isspace(*input))
            input++;

        else if (*input)
        {
            if (*input == '"')
            {
                const char *quotedEnd = input + 1;
                while (*quotedEnd != '"')
                {
                    // Check for illegal escape characters
                    if (*quotedEnd == '\\')
                    {
                        switch (*(quotedEnd + 1))
                        {
                        case 'n':
                        case 'r':
                        case 't':
                        case '"':
                        case '\\':
                        case ' ':
                        case '|':
                        case '<':
                        case '>':
                            break;

                        default:
                            snprintf(errmsg, errmsg_sz, "Illegal escape character '?%c", *(quotedEnd + 1));
                            CL_free(tokens);
                            return NULL;
                        }
                    }

                    quotedEnd++;
                    if (*quotedEnd == '\0')
                    {
                        snprintf(errmsg, errmsg_sz, "Unterminated quote");
                        CL_free(tokens);
                        return NULL;
                    }
                }

                // Allocate a new token
                char *quotedWord = malloc(strlen(input) - 1);

                if (quotedWord == NULL)
                {
                    snprintf(errmsg, errmsg_sz, "Unable to allocate memory for quoted word");
                    CL_free(tokens);
                    return NULL;
                }
                // Add the token to the list
                Token tok = {TOK_QUOTED_WORD, quotedWord};

                CL_append(tokens, (Token)tok);
                input = quotedEnd + 1;
            }

            else if (*input == '<')
            {
                Token tok = {TOK_LESSTHAN, NULL};
                CL_append(tokens, (Token)tok);
                input++;
            }

            else if (*input == '>')
            {
                Token tok = {TOK_GREATERTHAN, NULL};
                CL_append(tokens, (Token)tok);
                input++;
            }

            else if (*input == '|')
            {
                Token tok = {TOK_PIPE, NULL};
                CL_append(tokens, (Token)tok);
                input++;
            }

            else
            {
                char *word = malloc(strlen(input) + 1);
                int i = 0;
                while (*input != '\0' && !isspace(*input) && *input != '<' && *input != '>' && *input != '|' && *input != '"')
                {
                    if (*input == '\\')
                    {
                        switch (*(input + 1))
                        {
                        case 'n':
                            word[i++] = '\n';
                            break;

                        case 'r':
                            word[i++] = '\r';
                            break;

                        case 't':
                            word[i++] = '\t';
                            break;

                        case '"':
                            word[i++] = '\"';
                            break;

                        case '\\':
                            word[i++] = '\\';
                            break;

                        case ' ':
                            word[i++] = ' ';
                            break;

                        case '|':
                            word[i++] = '|';
                            break;

                        case '<':
                            word[i++] = '<';
                            break;

                        case '>':
                            word[i++] = '>';
                            break;

                        default:
                            snprintf(errmsg, errmsg_sz, "Illegal escape character '?%c", *(input + 1));
                            free(word);
                            CL_free(tokens);
                            return NULL;
                        }
                        input += 2;
                    }
                    else
                    {
                        word[i++] = *input++;
                    }
                }
                word[i] = '\0';

                // globbing
                glob_t globbuf;
                glob(word, GLOB_NOCHECK, NULL, &globbuf);

                switch (*word)
                {
                case '*':
                case '?':
                case '[':
                    // * and ? are expanded to the matching filenames, and [] is expanded to the matching characters
                    glob(word, GLOB_NOCHECK, NULL, &globbuf);
                    break;

                case '~':
                    // Expanding ~ to the home directory
                    char *tilde_rep = malloc(sizeof(char) * (strlen(word) + strlen(getenv("HOME")) + 1));
                    strcpy(tilde_rep, word);
                    glob(tilde_rep, GLOB_TILDE_CHECK, NULL, &globbuf);
                    free(tilde_rep);
                    free(word);
                    break;

                default:
                    break;
                }

                if (globbuf.gl_pathc != 0)
                {
                    for (int i = 0; i < globbuf.gl_pathc; i++)
                    {
                        char *w = malloc(strlen(globbuf.gl_pathv[i]) + 1);

                        strcpy(w, globbuf.gl_pathv[i]);
                        Token tok = {TOK_WORD, w};
                        CL_append(tokens, (Token)tok);

                        w = NULL;
                    }

                    globfree(&globbuf);

                    // deallocate the memory
                    free(word);
                }
                else
                {
                    char *w = malloc(strlen(word) + 1);
                    strcpy(w, word);
                    Token tok = {TOK_WORD, w};

                    // add the token to the list
                    CL_append(tokens, (Token)tok);

                    // deallocate the memory
                    free(word);
                    free(w);
                }

                globfree(&globbuf);
            }
        }
    }

    // Return the final list of tokens
    return tokens;
}

// Documented in .h file
TokenType TOK_next_type(CList tokens)
{
    if (tokens == NULL)
        return EMPTY_TOKEN.type;

    if (tokens->head != NULL)
        return tokens->head->tok_elt.type;

    return EMPTY_TOKEN.type;
}

// Documented in .h file
Token TOK_next(CList tokens)
{
    Token nextToken = {TOK_WORD, NULL};
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

    if (tok.type == EMPTY_TOKEN.type && tok.text == EMPTY_TOKEN.text)
        printf("%s: %d %s\n", (char *)data, pos, TT_to_str(tok.type));
    else
        printf("%s: %d %s %s\n", (char *)data, pos, TT_to_str(tok.type), tok.text);
}

// Documented in .h file
void TOK_print(CList tokens)
{
    if (tokens == NULL)
        return;

    // For debugging: Prints the list of tokens, one per line
    CL_foreach(tokens, print_element, "DEBUG OUTPUT");
}