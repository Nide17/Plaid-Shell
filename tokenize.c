/*
 * tokenize.c
 *
 * Functions to tokenize and manipulate lists of tokens
 *
 * Author: Niyomwungeri Parmenide Ishimwe <parmenin@andrew.cmu.edu>
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
CList TOK_tokenize_input(const char *user_input, char *errmsg, size_t errmsg_sz)
{
    // clear the error message
    errmsg[0] = '\0';
    CList tokens = CL_new();
    glob_t globbuf;

    while (user_input != NULL && *user_input != '\0')
    {
        // Skip any leading spaces
        if (isspace(*user_input))
            user_input++;

        else if (*user_input)
        {
            if (*user_input == '<')
            {
                Token tok = {TOK_LESSTHAN, NULL};
                CL_append(tokens, (Token)tok);
                user_input++;
            }

            else if (*user_input == '>')
            {
                Token tok = {TOK_GREATERTHAN, NULL};
                CL_append(tokens, (Token)tok);
                user_input++;
            }

            else if (*user_input == '|')
            {
                Token tok = {TOK_PIPE, NULL};
                CL_append(tokens, (Token)tok);
                user_input++;
            }

            // Check for a quoted word
            else if (*user_input == '"')
            {
                char *quoted_word = malloc(strlen(user_input) - 1);
                const char *end_quoted = user_input + 1;

                while (*end_quoted != '"')
                {
                    // Check for illegal escape characters
                    if (*end_quoted == '\\')
                    {
                        if (*(end_quoted + 1) != 'n' && *(end_quoted + 1) != 'r' && *(end_quoted + 1) != 't' && *(end_quoted + 1) != '"' && *(end_quoted + 1) != '\\' && *(end_quoted + 1) != ' ' && *(end_quoted + 1) != '|' && *(end_quoted + 1) != '<' && *(end_quoted + 1) != '>')
                        {
                            snprintf(errmsg, errmsg_sz, "Illegal escape character '%c", *(end_quoted + 1));
                            CL_free(tokens);
                            free(quoted_word);
                            return NULL;
                        }
                    }

                    end_quoted++;

                    // if we reach the end of the string without finding the closing quote, return an error
                    if (*end_quoted == '\0')
                    {
                        snprintf(errmsg, errmsg_sz, "Unterminated quote");
                        free(quoted_word);
                        CL_free(tokens);
                        return NULL;
                    }
                }

                // Else, we found the closing quote, so copy the quoted word into a new string
                char *w = malloc(end_quoted - user_input);
                if (w == NULL)
                {
                    snprintf(errmsg, errmsg_sz, "Unable to allocate memory for quoted word");
                    CL_free(tokens);
                    return NULL;
                }

                snprintf(w, end_quoted - user_input, "%s", user_input + 1);
                Token tok = {TOK_QUOTED_WORD, w};
                CL_append(tokens, (Token)tok);

                // deallocate the memory
                free(quoted_word);

                // move the pointer to the next character after the closing quote
                user_input = end_quoted + 1;
            }

            // Check for a word
            else
            {
                char *word = malloc(strlen(user_input) + 1);
                int i = 0;
                while (*user_input != '\0' && !isspace(*user_input) && *user_input != '<' && *user_input != '>' && *user_input != '|' && *user_input != '"')
                {
                    if (*user_input == '\\')
                    {
                        switch (*(user_input + 1))
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
                            snprintf(errmsg, errmsg_sz, "Illegal escape character '%c", *(user_input + 1));
                            CL_free(tokens);
                            free(word);

                            return NULL;
                        }
                        user_input += 2;
                    }
                    else
                    {
                        word[i++] = *user_input++;
                    }
                }
                word[i] = '\0';

                // Check for globbing characters
                switch (*word)
                {
                case '*':
                case '?':
                case '[':
                    glob(word, GLOB_NOCHECK, NULL, &globbuf);
                    break;

                case '~':
                    char *tilde_rep = malloc(sizeof(char) * (strlen(word) + strlen(getenv("HOME")) + 1));
                    strcpy(tilde_rep, word);
                    glob(tilde_rep, GLOB_TILDE_CHECK, NULL, &globbuf);
                    free(tilde_rep);
                    tilde_rep = NULL;
                    break;

                default:
                    glob(word, GLOB_TILDE_CHECK, NULL, &globbuf);
                    break;
                }

                if (globbuf.gl_pathc != 0)
                {
                    for (int i = 0; i < globbuf.gl_pathc; i++)
                    {
                        char *w = strdup(globbuf.gl_pathv[i]);
                        Token tok = {TOK_WORD, w};
                        CL_append(tokens, tok);
                    }
                }
                else
                {
                    // If no matches found, add the original word to tokens
                    Token tok = {TOK_WORD, strdup(word)};
                    CL_append(tokens, tok);

                    // deallocate the memory
                    if (word != NULL)
                    {
                        free(word);
                        word = NULL;
                    }
                }

                if (globbuf.gl_pathc != 0)
                    globfree(&globbuf);

                if (word != NULL)
                {
                    free(word);
                    word = NULL;
                }
            }
        }
    }

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