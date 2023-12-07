/*
 * pipeline.h
 *
 * Abstract Syntax Tree for the pipeline data structure
 *
 * Author: Niyomwungeri Parmenide Ishimwe <parmenin@andrew.cmu.edu>
 */
#ifndef PIPELINE_H
#define PIPELINE_H

#include <stdbool.h>


typedef struct pipeline pipeline_t;


/*
 * Create a new pipeline node.
 *
 * Parameters:
 *  command: the command to execute
 *  input: the input file
 *  output: the output file
 *  next: the next pipeline node
 * 
 * Returns:
 *  the new pipeline node
 */
pipeline_t *pipeline_new(char *command, char *input, char *output, pipeline_t *next);


/*
 * Free a pipeline node.
 *
 * Parameters:
 *  pipeline: the pipeline node to free
 * 
 * Returns:
 *  None
 */
void pipeline_free(pipeline_t *pipeline);

/*
 * Set the command of a pipeline node.
 *
 * Parameters:
 *  pipeline: the pipeline node
 *  command: the command to set
 * 
 * Returns:
 *  None
 */
void pipeline_set_command(pipeline_t *pipeline, char *command);


/*
 * Set the input file of a pipeline node.
 *
 * Parameters:
 *  pipeline: the pipeline node
 *  input: the input file to set
 * 
 * Returns:
 *  None
 */
void pipeline_set_input(pipeline_t *pipeline, char *input);


/*
 * Set the output file of a pipeline node.
 *
 * Parameters:
 *  pipeline: the pipeline node
 *  output: the output file to set
 * 
 * Returns:
 *  None
 */
void pipeline_set_output(pipeline_t *pipeline, char *output);

/*
 * Get the input file of a pipeline node.
 *
 * Parameters:
 *  pipeline: the pipeline node
 *
 * Returns:
 *  the input file of the pipeline node
 */
char *pipeline_get_input(pipeline_t *pipeline);

/*
 * Get the output file of a pipeline node.
 *
 * Parameters:
 *  pipeline: the pipeline node
 *
 * Returns:
 *  the output file of the pipeline node
 */
char *pipeline_get_output(pipeline_t *pipeline);

/*
 * Print the contents of a command to stdout
 *
 * Parameters:
 *  command: the command to print
 * 
 * Returns:
 *  None
 */
void print_command(char *command);

/*
 * Compares two commands
 *
 * Parameters:
 *  command1: the first command
 *  command2: the second command
 * 
 * Returns:
 *  true if the commands are equal, false otherwise
 */
bool compare_commands(char *command1, char *command2);

/*
 * Returns true if this command is empty, false otherwise.  An "empty" command has:
 *    input = stdin
 *    output = stdout
 *    no arguments
 * 
 * Parameters:
 *  command: the command to check
 * 
 * Returns:
 *  true if the command is empty, false otherwise
 */
bool is_empty_command(char *command);

/*
 * Return the count of arguments on this command
 *
 * Parameters:
 *  command: the command to check
 * 
 * Returns:
 *  the count of arguments on this command
 */
int count_args(char *command);

/*
 * Append a new argument to this command.
 *
 * Parameters:
 *  command: the command to append to
 *  arg: the argument to append
 * 
 * Returns:
 *  the new command
 */
char *append_arg(char *command, char *arg);

/*
 * Get a pointer to the NULL-terminated argv vector for this command
 *
 * Parameters:
 *   cmd     The command to get the argv vector for
 * 
 * Returns:
 *  A pointer to the NULL-terminated argv vector for this command
 */
char **get_argv(char *cmd);