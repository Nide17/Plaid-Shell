/*
 * pipeline.h
 *
 * Abstract Syntax Tree for the pipeline data structure
 *
 * Author: Niyomwungeri Parmenide Ishimwe <parmenin@andrew.cmu.edu>
 */

// For plaidsh, a pipeline is an abstract syntax tree. that has the following features:
// § The caller can set or get an input file for the whole pipeline; it is set to stdin by default
// § The caller can set or get an output file for the whole pipeline; it is set to stdin by default
// § The caller can add one or more commands, which are understood to be piped together(that is, the output of the first command is piped to the input of the second command, and so on)
// § Each command can have an arbitrary number of arguments, which can be added one - by - one
#ifndef PIPELINE_H
#define PIPELINE_H
#define MAX_ARGS 10

#include <stdbool.h>

struct pipeline_node // pipeline node is a command with args, input file, output file, and a pointer to the next pipeline node
{
    char *command;
    char *args[MAX_ARGS];
    char *input;
    char *output;
    struct pipeline_node *next;
};

struct pipeline // pipeline is a linked list of pipeline nodes
{
    struct pipeline_node *head;
    int length;
};

typedef struct pipeline pipeline_t; // pipeline_t is a pointer to a pipeline

/*
 * Create a new pipeline object.
 *
 * Parameters:
 *  None
 *
 * Returns:
 *  New pipeline object
 *
 * Note:
 * The pipeline object is a linked list of pipeline nodes.
 * Each pipeline node contains a command, input file, output file,
 * and a pointer to the next pipeline node.
 * The last pipeline node has a NULL next pointer.
 * The new created pipeline object must be freed by the caller.
 */
pipeline_t *pipeline_new();

/*
 * Free a pipeline object.
 *
 * Parameters:
 *  pipeline: the pipeline object to free
 *
 * Returns:
 *  None
 */
void pipeline_free(pipeline_t *pipeline);

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
void pipeline_set_input(pipeline_t *pipeline, char *input);

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
void pipeline_set_output(pipeline_t *pipeline, char *output);

/*
 * Get the input file of a pipeline object.
 *
 * Parameters:
 *  pipeline: the pipeline object
 *
 * Returns:
 *  the input file of the pipeline object
 */
char *pipeline_get_input(pipeline_t *pipeline);

/*
 * Get the output file of a pipeline object.
 *
 * Parameters:
 *  pipeline: the pipeline object
 *
 * Returns:
 *  the output file of the pipeline object
 */
char *pipeline_get_output(pipeline_t *pipeline);

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
void pipeline_add_command(pipeline_t *pipeline, char *command);

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
char *pipeline_get_command(pipeline_t *pipeline, int index);

/*
 * Get the number of commands in a pipeline object.
 *
 * Parameters:
 *  pipeline: the pipeline object
 *
 * Returns:
 *  the number of commands in the pipeline object
 */
int pipeline_command_count(pipeline_t *pipeline);

/*
 * Print the contents of a pipeline object to stdout
 *
 * Parameters:
 *  pipeline: the pipeline object to print
 *
 * Returns:
 *  None
 */
void pipeline_print(pipeline_t *pipeline);

#endif /* PIPELINE_H */