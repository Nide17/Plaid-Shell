/**
 * parser.c
 *
 * Functions to parse a list of tokens into a pipeline object.
 *
 * Contributor: Niyomwungeri Parmenide Ishimwe <parmenin@andrew.cmu.edu>
 */

#include <assert.h> // assert
#include <stdlib.h> // free/malloc
#include <stdio.h>  // printf
#include <string.h> // strcmp

#include "parser.h"
#include "pipeline.h"
#include "token.h"
#include "clist.h"
#include "tokenize.h"

// Documented in .h file
pipeline_t *parse_tokens(CList tokens)
{
    // allocate a new pipeline object
    pipeline_t *pipeline = pipeline_new();

    // pipeline node
    pipeline_node_t *node = NULL;

    // add each token to the current pipeline node until a pipe token is found
    for (int i = 0; i < tokens->length; i++)
    {
        // get the nth token from the list
        Token tok = CL_nth(tokens, i);

        if (tok.type == TOK_PIPE || tok.type == TOK_END)
        {
            // add the current node to the pipeline when encountering a pipe or end token
            if (node != NULL)
            {
                pipeline_add_node(pipeline, node);
                node = NULL;
            }
        }
        else
        {
            // create a new node for each command
            if (node == NULL)
            {
                node = pipeline_node_new();
                assert(node != NULL);
            }

            // add the token to the current node
            pipeline_node_add_arg(node, tok.text);
        }
    }

    // Add the final node if it exists after the loop ends
    if (node != NULL)
        pipeline_add_node(pipeline, node);

    // return the new pipeline object
    return pipeline;
}