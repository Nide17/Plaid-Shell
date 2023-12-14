/**
 * parser.c
 *
 * Functions to parse a list of tokens into a pipeline object.
 *
 * Contributor: Niyomwungeri Parmenide Ishimwe <parmenin@andrew.cmu.edu>
 */
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glob.h>

#include "parser.h"
#include "pipeline.h"
#include "token.h"
#include "clist.h"
#include "tokenize.h"

// Documented in .h file
pipeline_t *parse_tokens(CList tokens, char *errmsg, size_t errmsg_sz)
{
    // clear the error message
    errmsg[0] = '\0';

    pipeline_t *pipeline = pipeline_new();

    // pipeline node
    pipeline_cmd_t *node = NULL;

    // add each token to the current pipeline
    for (int i = 0; i < tokens->length; i++)
    {
        // get the nth token from the list
        Token tok = CL_nth(tokens, i);

        if (tok.type == TOK_WORD || tok.type == TOK_QUOTED_WORD)
        {
            // if the current pipeline node is NULL, create a new one
            if (node == NULL)
            {
                node = pipeline_cmd_new(tok.type);

                // add the token to the new node
                pipeline_cmd_add_arg(node, tok.text);

                // there are next tokens that are words add them to the args
                while (i + 1 < tokens->length)
                {
                    Token next_tok = CL_nth(tokens, i + 1);
                    if (next_tok.type == TOK_WORD || next_tok.type == TOK_QUOTED_WORD)
                    {
                        pipeline_cmd_add_arg(node, next_tok.text);
                        i++;
                    }
                    else
                    {
                        break;
                    }
                }

                // add the new node to the pipeline
                pipeline_add_command(pipeline, node);
            }
        }

        else if (tok.type == TOK_PIPE)
        {
            // add an empty node to the pipeline
            node = pipeline_cmd_new(tok.type);
            pipeline_add_command(pipeline, node);

            // if the next token is a not a word, raise an error
            if (i == 0 || i + 1 >= tokens->length || CL_nth(tokens, i + 1).type == TOK_PIPE)
            {
                snprintf(errmsg, errmsg_sz, "No command specified");
                pipeline_free(pipeline);
                return NULL;
            }

            // clear the current node
            node = NULL;
        }

        else if (tok.type == TOK_LESSTHAN)
        {
            // if the current pipeline node is NULL, create a new one
            node = pipeline_cmd_new(tok.type);
            pipeline_add_command(pipeline, node);

            if (i + 1 >= tokens->length || CL_nth(tokens, i + 1).type != TOK_WORD)
            {
                snprintf(errmsg, errmsg_sz, "Expect filename after redirection");
                return NULL;
            }

            // set the input file for the pipeline
            if (i + 1 < tokens->length)
            {
                Token nextTok = CL_nth(tokens, i + 1);
                if (nextTok.type == TOK_WORD || nextTok.type == TOK_QUOTED_WORD)
                {
                    pipeline_set_input(pipeline, nextTok.text);

                    // Input 'echo < file1 <file2': Expected 'Multiple redirection'
                    if (i + 2 < tokens->length)
                    {
                        Token nextTok2 = CL_nth(tokens, i + 2);
                        if (nextTok2.type == TOK_LESSTHAN)
                        {
                            snprintf(errmsg, errmsg_sz, "Multiple redirection");
                            pipeline_free(pipeline);
                            return NULL;
                        }
                    }
                    i++; // skip the next token since it has been processed

                    // clear the current node
                    node = NULL;
                }
            }
        }

        else if (tok.type == TOK_GREATERTHAN)
        {
            // if the current pipeline node is NULL, create a new one
            node = pipeline_cmd_new(tok.type);
            pipeline_add_command(pipeline, node);

            if (i + 1 >= tokens->length || CL_nth(tokens, i + 1).type != TOK_WORD)
            {
                snprintf(errmsg, errmsg_sz, "Expect filename after redirection");
                return NULL;
            }

            // set the output file for the pipeline
            if (i + 1 < tokens->length)
            {
                Token nextTok = CL_nth(tokens, i + 1);
                if (nextTok.type == TOK_WORD || nextTok.type == TOK_QUOTED_WORD)
                {
                    pipeline_set_output(pipeline, nextTok.text);

                    if (i + 2 < tokens->length)
                    {
                        Token nextTok2 = CL_nth(tokens, i + 2);
                        if (nextTok2.type == TOK_GREATERTHAN)
                        {
                            snprintf(errmsg, errmsg_sz, "Multiple redirection");
                            pipeline_free(pipeline);
                            return NULL;
                        }
                    }

                    i++; // skip the next token since it has been processed

                    // clear the current node
                    node = NULL;
                }
            }
        }
    }
    return pipeline;
}