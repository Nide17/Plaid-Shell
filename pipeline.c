/*
 * pipeline.c
 *
 * Functions to create and manipulate a pipeline object.
 *
 * Author: Niyomwungeri Parmenide Ishimwe <parmenin@andrew.cmu.edu>
 */

#include <assert.h> // assert
#include <stdlib.h> // free/malloc
#include <stdio.h>  // printf
#include <string.h> // strcmp

#include "pipeline.h"

// Documented in .h file
pipeline_cmd_t *pipeline_cmd_new(TokenType type)
{
    // allocate a new pipeline node
    pipeline_cmd_t *node = (pipeline_cmd_t *)malloc(sizeof(pipeline_cmd_t));
    assert(node != NULL);

    // initialize the pipeline node
    node->type = type;
    node->next = NULL;

    if (type == TOK_WORD || type == TOK_QUOTED_WORD)
    {
        // initialize the arguments
        for (int i = 0; i < MAX_ARGS; i++)
            node->args[i] = NULL;
    }

    // return the new pipeline node
    return node;
}

// Documented in .h file
void pipeline_node_free(pipeline_cmd_t *node)
{
    assert(node != NULL);

    // free the pipeline node
    free(node);
}

// Documented in .h file
pipeline_t *pipeline_new()
{
    // allocate a new pipeline object
    pipeline_t *pipeline = (pipeline_t *)malloc(sizeof(pipeline_t));
    assert(pipeline != NULL);

    // initialize the pipeline object
    pipeline->head = NULL;
    pipeline->length = 0;
    pipeline->input = NULL;
    pipeline->output = NULL;

    // return the new pipeline object
    return pipeline;
}

// Documented in .h file
void pipeline_free(pipeline_t *pipeline)
{
    if (!pipeline)
        return;

    pipeline_cmd_t *curr_node = pipeline->head;

    // traverse the pipeline, freeing each node
    while (curr_node != NULL)
    {
        // save a pointer to the next node
        pipeline_cmd_t *next_node = curr_node->next;

        // free the current node
        pipeline_node_free(curr_node);

        // move on to the next node
        curr_node = next_node;
    }

    // free the pipeline object
    free(pipeline);
}

// Documented in .h file
void pipeline_set_input(pipeline_t *pipeline, char *input)
{
    assert(pipeline != NULL);
    pipeline->input = input;
}

// Documented in .h file
void pipeline_set_output(pipeline_t *pipeline, char *output)
{
    assert(pipeline != NULL);
    pipeline->output = output;
}

// Documented in .h file
char *pipeline_get_input(pipeline_t *pipeline)
{
    assert(pipeline != NULL);
    return pipeline->input;
}

// Documented in .h file
char *pipeline_get_output(pipeline_t *pipeline)
{
    assert(pipeline != NULL);
    return pipeline->output;
}

// Documented in .h file
void pipeline_add_command(pipeline_t *pipeline, pipeline_cmd_t *node)
{
    assert(pipeline != NULL);
    assert(node != NULL);

    // add the node to the pipeline: this is the first node in the pipeline
    if (pipeline->head == NULL)
        pipeline->head = node;

    else
    {
        // find the last node in the pipeline
        pipeline_cmd_t *this_node = pipeline->head;

        while (this_node->next != NULL)
            this_node = this_node->next;

        // add the node to the end of the pipeline
        this_node->next = node;
    }

    // increment the length of the pipeline
    pipeline->length++;
}

// Documented in .h file
void pipeline_cmd_add_arg(pipeline_cmd_t *node, char *arg)
{
    assert(node != NULL);
    if (node->type == TOK_WORD || node->type == TOK_QUOTED_WORD)
    {
        // find the first NULL argument
        int i = 0;
        while (node->args[i] != NULL)
            i++;

        // add the argument
        node->args[i] = arg;
    }
}

// Documented in .h file
pipeline_cmd_t *pipeline_get_command(pipeline_t *pipeline, int index)
{
    assert(pipeline != NULL);
    assert(index >= 0 && index < pipeline->length);

    // traverse the pipeline until we find the node at the given index
    pipeline_cmd_t *this_node = pipeline->head;

    for (int i = 0; i < index; i++)
        this_node = this_node->next;

    // return the command node at the given index
    return this_node;
}

// Documented in .h file
void pipeline_print(pipeline_t *pipeline)
{
    assert(pipeline != NULL);

    printf("\nThe Pipeline:\n");

    // print the input file
    printf("Input: %s\n", pipeline->input);

    // print the output file
    printf("Output: %s\n", pipeline->output);

    // print the commands
    pipeline_cmd_t *this_node = pipeline->head;
    while (this_node != NULL)
    {
        if (this_node->type != TOK_WORD && this_node->type != TOK_QUOTED_WORD)
        {
            printf("Command: {null}\n");
            this_node = this_node->next;
            continue;
        }

        // print the command
        printf("Command: %s - args: ", this_node->args != NULL ? this_node->args[0] : "NULL");

        // print the arguments
        int i = 1;
        while (this_node->args[i] != NULL)
        {
            printf("%s ", this_node->args[i]);
            i++;
        }

        // advance to the next node
        this_node = this_node->next;

        // print a newline
        printf("\n");
    }

    // print the length
    printf("Length: %d\n", pipeline->length);

    printf("End of the pipeline\n\n");
}