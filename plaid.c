/*
 * plaid.c
 *
 * A small shell that can execute commands with pipes and redirection.
 *
 * Author: Niyomwungeri Parmenide ISHIMWE <parmenin@andrew.cmu.edu>
 */
#include <readline/readline.h>
#include <readline/history.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <assert.h>
#include <fcntl.h>

#include "clist.h"
#include "tokenize.h"
#include "token.h"
#include "pipeline.h"
#include "parser.h"

/*
 * Executes a pipeline of commands
 *
 * Parameters:
 *   pipeline   The pipeline to execute
 *
 * Returns:
 *   The exit status of the last command in the pipeline
 */
int execute_pipeline(pipeline_t *pipeline)
{
    int num_commands = pipeline->length;
    int status;
    pid_t pid;

    int prev_pipe[2];
    int cur_pipe[2];

    pipeline_cmd_t *cur_node = pipeline->head;

    // Create the pipe for the first command
    if (pipe(prev_pipe) == -1)
    {
        perror("pipe");
        exit(1);
    }

    // Execute each command in the pipeline
    for (int i = 0; i < num_commands; i++)
    {
        // Create the pipe for the next command
        if (i < num_commands - 1 && pipe(cur_pipe) == -1)
        {
            perror("pipe");
            exit(1);
        }

        // Fork a new process for the current command
        pid = fork();

        if (pid == 0)
        {
            // Child process that runs the command 
            if (i > 0)
            {
                // if it is not the first command, read from the previous pipe
                dup2(prev_pipe[0], STDIN_FILENO);
                close(prev_pipe[0]);
                close(prev_pipe[1]);
            }

            if (i < num_commands - 1)
            {
                // if it is not the last command, write to the next pipe
                dup2(cur_pipe[1], STDOUT_FILENO);
                close(cur_pipe[0]);
                close(cur_pipe[1]);
            }

            // Redirect input and output if necessary 
            if (pipeline_get_input(pipeline) != NULL)
            {
                // Open the input file for reading 
                int fd = open(pipeline_get_input(pipeline), O_RDONLY);
                if (fd == -1)
                {
                    perror(pipeline_get_input(pipeline));
                    exit(1);
                }

                // Redirect stdin to the input file
                dup2(fd, STDIN_FILENO);
                close(fd);
            }

            // Open the output file for writing
            if (pipeline_get_output(pipeline) != NULL)
            {
                // Open the output file for writing
                int fd = open(pipeline_get_output(pipeline), O_WRONLY | O_CREAT | O_TRUNC, 0666);
                if (fd == -1)
                {
                    perror(pipeline_get_output(pipeline));
                    exit(1);
                }

                dup2(fd, STDOUT_FILENO);
                close(fd);
            }

            // Execute the command 
            if (cur_node->type == TOK_WORD)
            {
                // Built-in commands
                if (strcmp(cur_node->args[0], "author") == 0)
                {
                    int pipefd[2];
                    if (pipe(pipefd) == -1)
                    {
                        perror("pipe");
                        exit(1);
                    }

                    pid_t next_command_pid;

                    // Fork a new process for the next command after author
                    next_command_pid = fork();

                    if (next_command_pid == 0)
                    {
                        // Child process
                        close(pipefd[0]);
                        dup2(pipefd[1], STDOUT_FILENO);
                        close(pipefd[1]);
                    }
                    else if (next_command_pid > 0)
                    {
                        // Parent process
                        close(pipefd[1]);
                        dup2(pipefd[0], STDIN_FILENO);
                        close(pipefd[0]);

                        // Execute the author command
                        printf("Niyomwungeri Parmenide ISHIMWE\n");

                        exit(0);
                    }
                    else
                    {
                        perror("fork");
                        exit(1);
                    }

                    // Wait for the next command to finish
                    pid_t terminated_pid = waitpid(next_command_pid, &status, 0);

                    if (terminated_pid == -1)
                        exit(1);

                    return status;
                }

                else if (strcmp(cur_node->args[0], "pwd") == 0)
                {
                    char *cwd = getcwd(NULL, 0);
                    printf("%s\n", cwd);
                    free(cwd);
                    return 0;
                }
                else if (strcmp(cur_node->args[0], "cd") == 0)
                {
                    if (cur_node->args[1] != NULL)
                    {
                        if (chdir(cur_node->args[1]) != 0)
                        {
                            perror("chdir");
                            exit(1);
                        }
                    }
                    else
                        chdir(getenv("HOME"));
                    return 0;
                }

                // Execute the command if it is not a built-in command, pipe or redirection
                else if (execvp(cur_node->args[0], cur_node->args) == -1)
                {
                    printf("%s: Command not found \n", cur_node->args[0]);
                    perror(cur_node->args[0]);
                    fprintf(stderr, "Child %d exited with status %d \n", pid, 2);
                    exit(1);
                }
            }

            // Close file descriptors for redirection
            if (pipeline_get_output(pipeline) != NULL)
                close(STDOUT_FILENO);

            if (pipeline_get_input(pipeline) != NULL)
                close(STDIN_FILENO);

            status = 0;
        }
        else if (pid > 0)
        {
            // Parent process
            if (i != 0)
            {
                close(prev_pipe[0]);
                close(prev_pipe[1]);
            }

            if (i < num_commands - 1)
            {
                prev_pipe[0] = cur_pipe[0];
                prev_pipe[1] = cur_pipe[1];
            }
        }
        else
        {
            perror("fork");
            exit(1);
        }

        // Advance to the next node
        cur_node = cur_node->next;
    }

    // Close the last pipe in the parent process
    if (pid > 0 && num_commands > 1)
    {
        close(prev_pipe[0]);
        close(prev_pipe[1]);
    }

    // Wait for all the child processes to finish
    for (int i = 0; i < num_commands; i++)
    {
        pid_t terminated_pid = waitpid(-1, &status, 0);

        if (terminated_pid == -1)
            exit(1);
    }

    // Close the last pipe output
    if (num_commands > 1)
    {
        close(prev_pipe[0]);
        close(prev_pipe[1]);
    }

    return status;
}

int main()
{
    CList tokens = NULL;
    pipeline_t *pipeline = NULL;
    char errmsg[100];
    char *user_input = NULL;
    int status;

    fprintf(stdout, "Welcome to Plaid Shell!\n");
    const char *terminal = "#? ";

    while (1)
    {
        // read the user input
        user_input = readline(terminal);

        // if the user entered nothing or spaces, loop again
        while (user_input != NULL && *user_input != '\0' && isspace(*user_input))
            user_input++;

        if (user_input == NULL || *user_input == '\0')
            continue;

        // store history of commands
        add_history(user_input);

        // tokenize the user input
        tokens = TOK_tokenize_input(user_input, errmsg, sizeof(errmsg));

        // check for errors while tokenizing
        if (strlen(errmsg) > 0)
        {
            printf("%s\n", errmsg);
            CL_free(tokens);
            continue;
        }

        // if there are no tokens, loop again
        if (tokens->length == 0)
        {
            CL_free(tokens);
            continue;
        }

        // build the pipeline from the list of tokens
        pipeline = parse_tokens(tokens, errmsg, sizeof(errmsg));

        if (pipeline == NULL)
        {
            printf("%s\n", errmsg);
            CL_free(tokens);
            continue;
        }

        if (pipeline->head->args[0] && (strcmp(pipeline->head->args[0], "exit") == 0 || strcmp(pipeline->head->args[0], "quit") == 0))
        {
            CL_free(tokens);
            pipeline_free(pipeline);
            free(user_input);
            exit(0);
        }

        if (strlen(errmsg) > 0)
        {
            printf("%s\n", errmsg);
            CL_free(tokens);
            pipeline_free(pipeline);
            continue;
        }

        // execute the pipeline
        status = execute_pipeline(pipeline);

        // free the memory
        CL_free(tokens);
        pipeline_free(pipeline);
        free(user_input);
        user_input = NULL;
    }

    if (user_input != NULL)
    {
        free(user_input);
        user_input = NULL;
    }

    exit(status);
}