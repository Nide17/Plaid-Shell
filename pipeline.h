/*
 * pipeline.h
 *
 * Abstract Syntax Tree for the pipeline data structure
 *
 * Author: Niyomwungeri Parmenide Ishimwe <parmenin@andrew.cmu.edu>
 */
#ifndef PIPELINE_H
#define PIPELINE_H
#define MAX_ARGS 50

#include "token.h"

struct pipeline_node // pipeline node is a command with args, input file, output file, and a pointer to the next pipeline node
{
    TokenType type;
    char *args[MAX_ARGS];
    struct pipeline_node *next;
};

typedef struct pipeline_node pipeline_cmd_t; // pipeline_cmd_t is a pointer to a pipeline node
struct pipeline                              // pipeline is a linked list of pipeline nodes
{
    pipeline_cmd_t *head;
    int length;
    char *input;
    char *output;
};

typedef struct pipeline pipeline_t; // pipeline_t is a pointer to a pipeline

/*
 * Create a new node for the pipeline object.
 *
 * Parameters:
 *  None
 *
 * Returns:
 *  New node for the pipeline object
 */
pipeline_cmd_t *pipeline_cmd_new();


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
 * Add a new node (command with args) to a pipeline object.
 *
 * Parameters:
 *  pipeline: the pipeline object
 *  node: the node(command with args) to add
 *
 * Returns:
 *  None
 */
void pipeline_add_command(pipeline_t *pipeline, pipeline_cmd_t *node);

/*
 * Add a new argument to the current node in a pipeline object.
 *
 * Parameters:
 *  node: the current node in the pipeline object
 *  arg: the argument to add
 *
 * Returns:
 *  None
 */
void pipeline_cmd_add_arg(pipeline_cmd_t *node, char *arg);

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
pipeline_cmd_t *pipeline_get_command(pipeline_t *pipeline, int index);

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