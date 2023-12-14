/*
 * plaidsh.c
 *
 * A small shell that can execute commands with pipes and redirection.
 *
 * Author: Howdy Pierce <howdy@sleepymoose.net>
 * Co-Author: Niyomwungeri Parmenide ISHIMWE <parmenin@andrew.cmu.edu>
 */
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
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
 */
void execute_pipeline(pipeline_t *pipeline)
{
    int num_commands = pipeline->length;
    int status;
    pid_t pid;

    int prev_pipe[2];
    int cur_pipe[2];

    pipeline_cmd_t *cur_node = pipeline->head;

    // Create the first pipe
    if (pipe(prev_pipe) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < num_commands; i++)
    {
        if (i < num_commands - 1 && pipe(cur_pipe) == -1)
        {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        pid = fork();

        if (pid == 0)
        {
            // Child process
            if (i != 0)
            {
                dup2(prev_pipe[0], STDIN_FILENO);
                close(prev_pipe[0]);
                close(prev_pipe[1]);
            }

            if (i < num_commands - 1)
            {
                dup2(cur_pipe[1], STDOUT_FILENO);
                close(cur_pipe[0]);
                close(cur_pipe[1]);
            }

            if (pipeline_get_output(pipeline) != NULL)
            {
                int fd = open(pipeline_get_output(pipeline), O_WRONLY | O_CREAT | O_TRUNC, 0666);
                if (fd == -1)
                {
                    perror(pipeline_get_output(pipeline));
                    exit(EXIT_FAILURE);
                }

                dup2(fd, STDOUT_FILENO);
                close(fd);
            }

            if (pipeline_get_input(pipeline) != NULL)
            {
                int fd = open(pipeline_get_input(pipeline), O_RDONLY);
                if (fd == -1)
                {
                    perror(pipeline_get_input(pipeline));
                    exit(EXIT_FAILURE);
                }

                dup2(fd, STDIN_FILENO);
                close(fd);
            }

            if (cur_node->type == TOK_WORD)
            {
                // Built-in commands
                if (strcmp(cur_node->args[0], "author") == 0)
                {
                    printf("Niyomwungeri Parmenide Ishimwe\n");
                    return;
                }
                else if (strcmp(cur_node->args[0], "pwd") == 0)
                {
                    char *cwd = getcwd(NULL, 0);
                    printf("%s\n", cwd);
                    free(cwd);
                    return;
                }
                else if (strcmp(cur_node->args[0], "cd") == 0)
                {
                    if (cur_node->args[1] != NULL)
                    {
                        if (chdir(cur_node->args[1]) != 0)
                            perror("chdir");
                    }
                    else
                        chdir(getenv("HOME"));
                    return;
                }

                // Execute the command if it is not a built-in command, pipe or redirection
                else if (execvp(cur_node->args[0], cur_node->args) == -1)
                {
                    printf("%s: Command not found \n", cur_node->args[0]);
                    fprintf(stderr, "Child %d exited with status %d \n", pid, 2);
                    exit(EXIT_FAILURE);
                }
            }

            // Close file descriptors for redirection
            if (pipeline_get_output(pipeline) != NULL)
                close(STDOUT_FILENO);

            if (pipeline_get_input(pipeline) != NULL)
                close(STDIN_FILENO);
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
            exit(EXIT_FAILURE);
        }

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
        {
            // perror("waitpid");
            exit(EXIT_FAILURE);
        }
        if (WIFEXITED(status))
        {
            // printf("Child process %d exited with status %d\n", terminated_pid, WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status))
        {
            // printf("Child process %d terminated by signal %d\n", terminated_pid, WTERMSIG(status));
        }
    }
    // printf("All child processes finished\n");

    // Close the last pipe output
    if (num_commands > 1)
    {
        close(prev_pipe[0]);
        close(prev_pipe[1]);
    }
}

int main()
{
    char *user_input = NULL;
    CList tokens = NULL;
    pipeline_t *pipeline = NULL;
    char errmsg[100];

    fprintf(stdout, "Welcome to Plaid Shell!\n");
    const char *terminal = "#? ";

    while (1)
    {
        user_input = readline(terminal);

        // if the user entered nothing or spaces, loop again
        while (user_input != NULL && *user_input != '\0' && isspace(*user_input))
            user_input++;

        if (user_input == NULL || *user_input == '\0')
            continue;

        add_history(user_input);

        tokens = TOK_tokenize_input(user_input, errmsg, sizeof(errmsg));

        if (strlen(errmsg) > 0)
        {
            printf("%s\n", errmsg);
            CL_free(tokens);
            free(user_input);
            continue;
        }

        if (tokens->length == 0)
        {
            CL_free(tokens);
            // free(user_input);
            continue;
        }

        pipeline = parse_tokens(tokens, errmsg, sizeof(errmsg));

        if (pipeline == NULL)
        {
            printf("%s\n", errmsg);
            CL_free(tokens);
            free(user_input);
            continue;
        }

        if (strcmp(pipeline->head->args[0], "exit") == 0 || strcmp(pipeline->head->args[0], "quit") == 0)
        {
            CL_free(tokens);
            pipeline_free(pipeline);
            free(user_input);
            break;
        }

        if (strlen(errmsg) > 0)
        {
            printf("%s\n", errmsg);
            CL_free(tokens);
            pipeline_free(pipeline);
            free(user_input);
            continue;
        }

        execute_pipeline(pipeline);
        CL_free(tokens);
        pipeline_free(pipeline);
        free(user_input);
    }

    return 0;
}