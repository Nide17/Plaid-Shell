/*
 * pipeline.c
 *
 * Author: Niyomwungeri Parmenide Ishimwe <parmenin@andrew.cmu.edu>
 */

#include <assert.h> // assert
#include <stdlib.h> // free/malloc
#include <stdio.h>  // printf
#include <string.h> // strcmp

#include "pipeline.h"

// For plaidsh, a pipeline is an abstract syntax tree. that has the following features:
// § The caller can set or get an input file for the whole pipeline; it is set to stdin by default
// § The caller can set or get an output file for the whole pipeline; it is set to stdin by default
// § The caller can add one or more commands, which are understood to be piped together(that is, the output of the first command is piped to the input of the second command, and so on)
// § Each command can have an arbitrary number of arguments, which can be added one - by - one

/*
 * Create a new node for the pipeline object.
 *
 * Parameters:
 *  None
 *
 * Returns:
 *  New node for the pipeline object
 */
pipeline_node_t *pipeline_node_new()
{
    // allocate a new pipeline node
    pipeline_node_t *node = (pipeline_node_t *)malloc(sizeof(pipeline_node_t));
    assert(node != NULL);

    // initialize the pipeline node
    node->next = NULL;

    // initialize the arguments
    for (int i = 0; i < MAX_ARGS; i++)
    {
        node->args[i] = NULL;
    }

    // return the new pipeline node
    return node;
}

/**
 * Free a pipeline node.
 * 
 * Parameters:
 *  node: the pipeline node to free
 * 
 * Returns:
 *  None
 */
void pipeline_node_free(pipeline_node_t *node)
{
    assert(node != NULL);

    // free the pipeline node
    free(node);
}

/*
 * Create a new pipeline object.
 *
 * Parameters:
 *  None
 *
 * Returns:
 *  New pipeline object
 */
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

/*
 * Free a pipeline object.
 *
 * Parameters:
 *  pipeline: the pipeline object to free
 *
 * Returns:
 *  None
 */
void pipeline_free(pipeline_t *pipeline)
{
    assert(pipeline != NULL);

    // free all the nodes in the pipeline
    pipeline_node_t *this_node = pipeline->head;

    // traverse the pipeline, freeing each node
    while (this_node != NULL)
    {
        // save a pointer to the next node
        pipeline_node_t *next_node = this_node->next;

        // free the current node
        pipeline_node_free(this_node);

        // move on to the next node
        this_node = next_node;
    }

    // free the pipeline object
    free(pipeline);
}

/*
 * Set the input file of a pipeline object.
 *
 * Parameters:
 *  pipeline: the pipeline object
 *  input: the input file to set
 *
 * Returns:
 *  None
 */
void pipeline_set_input(pipeline_t *pipeline, char *input)
{
    assert(pipeline != NULL);
    pipeline->input = input;
}

/*
 * Set the output file of a pipeline object.
 *
 * Parameters:
 *  pipeline: the pipeline object
 *  output: the output file to set
 *
 * Returns:
 *  None
 */
void pipeline_set_output(pipeline_t *pipeline, char *output)
{
    assert(pipeline != NULL);
    pipeline->output = output;
}

/*
 * Get the input file of a pipeline object.
 *
 * Parameters:
 *  pipeline: the pipeline object
 *
 * Returns:
 *  the input file of the pipeline object
 */
char *pipeline_get_input(pipeline_t *pipeline)
{
    assert(pipeline != NULL);
    return pipeline->input;
}

/*
 * Get the output file of a pipeline object.
 *
 * Parameters:
 *  pipeline: the pipeline object
 *
 * Returns:
 *  the output file of the pipeline object
 */
char *pipeline_get_output(pipeline_t *pipeline)
{
    assert(pipeline != NULL);
    return pipeline->output;
}

/*
 * Add a new command to a pipeline object.
 *
 * Parameters:
 *  pipeline: the pipeline object
 *  command: the command to add
 *
 * Returns:
 *  None
 */
void pipeline_add_command(pipeline_t *pipeline, char *command)
{
    assert(pipeline != NULL);

    // allocate a new node
    pipeline_node_t *new_node = pipeline_node_new();
    assert(new_node != NULL);

    // initialize the new node
    new_node->args[0] = command;
    new_node->next = NULL;

    // add the new node to the pipeline
    if (pipeline->head == NULL)
    {
        // this is the first node in the pipeline
        pipeline->head = new_node;
    }
    else
    {
        // find the last node in the pipeline
        pipeline_node_t *this_node = pipeline->head;
        while (this_node->next != NULL)
        {
            this_node = this_node->next;
        }

        // add the new node to the end of the pipeline
        this_node->next = new_node;
    }

    // increment the length of the pipeline
    pipeline->length++;
}

/*
 * Add a new node to a pipeline object.
 *
 * Parameters:
 *  pipeline: the pipeline object
 *  node: the node to add
 *
 * Returns:
 *  None
 */
void pipeline_add_node(pipeline_t *pipeline, pipeline_node_t *node)
{
    assert(pipeline != NULL);
    assert(node != NULL);

    // add the node to the pipeline
    if (pipeline->head == NULL)
    {
        // this is the first node in the pipeline
        pipeline->head = node;
    }
    else
    {
        // find the last node in the pipeline
        pipeline_node_t *this_node = pipeline->head;
        while (this_node->next != NULL)
        {
            this_node = this_node->next;
        }

        // add the new node to the end of the pipeline
        this_node->next = node;
    }

    // increment the length of the pipeline
    pipeline->length++;
}

/*
 * Add a new argument to the current node in a pipeline object.
 *
 * Parameters:
 *  pipeline: the pipeline object
 *  arg: the argument to add
 *
 * Returns:
 *  None
 */
void pipeline_node_add_arg(pipeline_node_t *node, char *arg)
{
    assert(node != NULL);

    // find the first NULL argument in the node
    int i = 0;
    while (node->args[i] != NULL)
    {
        i++;
    }

    // add the argument to the node
    node->args[i] = arg;
}

/*
 * Get the command at the given index in a pipeline object.
 *
 * Parameters:
 *  pipeline: the pipeline object
 *  index: the index of the command to get
 *
 * Returns:
 *  the command at the given index in the pipeline object
 */
char *pipeline_get_command(pipeline_t *pipeline, int index)
{
    assert(pipeline != NULL);
    assert(index >= 0 && index < pipeline->length);

    // traverse the pipeline until we find the node at the given index
    pipeline_node_t *this_node = pipeline->head;
    for (int i = 0; i < index; i++)
    {
        this_node = this_node->next;
    }

    // return the command at the given index
    return this_node->args[0];
}

/*
 * Print the contents of a pipeline object to stdout
 *
 * Parameters:
 *  pipeline: the pipeline object to print
 *
 * Returns:
 *  None
 */
void pipeline_print(pipeline_t *pipeline)
{
    assert(pipeline != NULL);

    // print the input file
    printf("Input: %s\n", pipeline->input);

    // print the output file
    printf("Output: %s\n", pipeline->output);

    // print the commands
    pipeline_node_t *this_node = pipeline->head;
    while (this_node != NULL)
    {
        // print the command
        printf("Command: %s\n", this_node->args[0]);

        // advance to the next node
        this_node = this_node->next;
    }

    // print the length
    printf("Length: %d\n", pipeline->length);
}