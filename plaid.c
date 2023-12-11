/*
 * plaidsh.c
 *
 * A small shell
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

#include "clist.h"
#include "tokenize.h"
#include "token.h"
#include "pipeline.h"
#include "parser.h"

#define MAX_ARGS 20

/*
 * Process an external command, by forking and executing
 * a child process, and waiting for the child to terminate
 *
 * Parameters:
 *   tok_len      The length of the argv vector
 *   argv[]    The argument vector
 *
 * Returns:
 *   The child's exit value, or -1 on error
 */
static int forkexec_cmd(char *argv[])
{
    // start a child process to execute the command
    pid_t pid = fork();

    // if the child process, execute the command
    if (pid == 0)
    {
        // execute the command if it only exists
        if (execvp(argv[0], argv) == -1)
        {
            printf("%s: Command not found \n", argv[0]);
            exit(-1);
        }

        // exit the child process
        exit(0);
    }

    // if the parent process, wait for the child to terminate
    else if (pid > 0)
    {
        int status;
        waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
    }
    else
    {
        printf("Error: fork failed \n");
        fprintf(stderr, "Child %d exited with status %d \n", pid, WEXITSTATUS(pid));
        return -1;
    }
}

/*
 * Executes a pipeline of commands
 *
 * Parameters:
 *   pipeline   The pipeline to execute
 */
void execute_pipeline(pipeline_t *pipeline)
{
    // if the pipeline is empty, return
    if (pipeline == NULL)
        return;

    // if the pipeline is not empty, execute the commands
    else
    {
        // execute each command in the pipeline
        for (int i = 0; i < pipeline->length; i++)
        {
            // get the command at the given index
            char *command = pipeline_get_command(pipeline, i);

            // if the command is exit or quit, exit the shell
            if (strcmp(command, "exit") == 0 || strcmp(command, "quit") == 0)
                exit(0);

            // if the command is author, print the author
            else if (strcmp(command, "author") == 0)
                printf("Niyomwungeri Parmenide Parmenide\n");

            // if the command is cd, change the directory
            else if (strcmp(command, "cd") == 0)
            {
                // Go to the directory specified by argv[1]
                if (chdir(pipeline->head->args[1]) == 0)
                    printf("Directory changed to %s \n", pipeline->head->args[1]);

                else
                    fprintf(stderr, "Error: cd command failed \n");
            }

            // if the command is pwd, print the current directory
            else if (strcmp(command, "pwd") == 0)
            {
                char currentDir[1024];
                getcwd(currentDir, sizeof(currentDir));

                // print the current directory
                printf("%s \n", currentDir);
            }

            // if the command is not a built-in command, execute it
            else
            {
                // fork and execute the command
                int status = forkexec_cmd(pipeline->head->args);

                // if the command failed, print an error message
                if (status == -1)
                    printf("Error: command failed \n");
            }
        }
    }
}

int main(int tok_len, char *argv[])
{
    char *input = NULL;
    CList tokens = NULL;         // list of tokens
    pipeline_t *pipeline = NULL; // pipeline of commands
    char errmsg[100];

    fprintf(stdout, "Welcome to Plaid Shell!\n");
    const char *prompt = "#? ";

    // main loop of the shell - read, parse, execute commands
    while (1)
    {
        // get the input from the user
        input = readline(prompt);

        // store the input in the history buffer
        add_history(input);

        if(!input || *input == '\0')
        {
            free(input);
            continue;
        }

        // tokenize the input into arguments
        tokens = TOK_tokenize_input(input, errmsg, sizeof(errmsg));

        // if the input was empty, go back to the prompt
        if (CL_length(tokens) == 0)
        {
            free(input);
            CL_free(tokens);
            continue;
        }

        // if the input was not empty, execute the command
        else
        {
            // add the tokens to the pipeline
            pipeline = parse_tokens(tokens);

            // execute the pipeline
            execute_pipeline(pipeline);
        }

        // free the input
        free(input);

        // free the tokens
        CL_free(tokens);

        // free the pipeline
        pipeline_free(pipeline);
    }

    return 0;
}