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
    pipeline_t *pipeline = (pipeline_t *)malloc(sizeof(pipeline_t));
    assert(pipeline != NULL);

    pipeline->head = NULL;
    pipeline->length = 0;

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
    if (pipeline == NULL)
        return;

    // deallocate all the nodes in the pipeline
    struct pipeline_node *this_node = pipeline->head;

    // traverse the pipeline, deallocating each node
    while (this_node != NULL)
    {
        // save a pointer to the next node
        struct pipeline_node *next_node = this_node->next;

        // deallocate this node
        free(this_node);

        // advance to the next node
        this_node = next_node;
    }

    // deallocate the pipeline object
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

    // create a new pipeline node
    struct pipeline_node *new_node = (struct pipeline_node *)malloc(sizeof(struct pipeline_node));
    assert(new_node != NULL);

    // set the command of the new node
    new_node->command = command;

    // set the input and output of the new node to NULL
    new_node->input = NULL;
    new_node->output = NULL;

    // set the next pointer of the new node to NULL
    new_node->next = NULL;

    // add the new node to the end of the pipeline
    if (pipeline->head == NULL)
    {
        // the pipeline is empty, so set the head to the new node
        pipeline->head = new_node;
    }
    else
    {
        // the pipeline is not empty, so find the last node
        struct pipeline_node *last_node = pipeline->head;
        
        while (last_node->next != NULL)
        {
            last_node = last_node->next;
        }

        // set the next pointer of the last node to the new node
        last_node->next = new_node;
    }

    // increment the length of the pipeline
    pipeline->length++;
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
    struct pipeline_node *this_node = pipeline->head;
    for (int i = 0; i < index; i++)
    {
        this_node = this_node->next;
    }

    // return the command of the node at the given index
    return this_node->command;
}

/*
 * Get the number of commands in a pipeline object.
 *
 * Parameters:
 *  pipeline: the pipeline object
 *
 * Returns:
 *  the number of commands in the pipeline object
 */
int pipeline_command_count(pipeline_t *pipeline)
{
    assert(pipeline != NULL);
    return pipeline->length;
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
    printf("input: %s\n", pipeline->input);

    // print the output file
    printf("output: %s\n", pipeline->output);

    // print the commands
    struct pipeline_node *this_node = pipeline->head;
    while (this_node != NULL)
    {
        // print the command
        printf("command: %s\n", this_node->command);

        // print the input file
        printf("input: %s\n", this_node->input);

        // print the output file
        printf("output: %s\n", this_node->output);

        // advance to the next node
        this_node = this_node->next;
    }
}