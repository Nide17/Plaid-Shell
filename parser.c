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
    assert(pipeline != NULL);

    // initialize the pipeline object
    pipeline->head = NULL;
    pipeline->length = 0;
    pipeline->input = NULL;
    pipeline->output = NULL;

    // pipeline node
    struct pipeline_node *node = pipeline_node_new();

    // add each token to the current pipeline node until a pipe token is found
    for (int i = 0; i < tokens->length; i++)
    {
        // get the nth token from the list
        Token tok = CL_nth(tokens, i);

        while (tok.type != TOK_PIPE && tok.type != TOK_END)
        {
            // add the token to the current node
            pipeline_node_add_arg(node, tok.text);
            i++;
            tok = CL_nth(tokens, i);
        }

        // add the current node to the pipeline
        pipeline_add_node(pipeline, node);
    }    

    // return the new pipeline object
    return pipeline;
}