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

// #include "parser.h"

#define MAX_ARGS 20

/*
 * Handles the exit or quit commands, by exiting the shell. Does not
 * return.
 *
 * Parameters:
 *   argc     The length of the argv vector
 *   argv[]   The argument vector
 */
static int fn_exit(int argc, char *argv[])
{
    // if exit or quit COMMAND - EXIT
    if ((argc == 1 && strcmp(argv[0], "exit") == 0) || (strcmp(argv[0], "quit") == 0))
        exit(0);

    return 0;
}

/*
 * Handles the author command, by printing the author of this program
 * to stdout
 *
 * Parameters:
 *   argc     The length of the argv vector
 *   argv[]   The argument vector, which is ignored
 *
 * Returns:
 *   0 on success, 1 on failure
 */
static int fn_author(int argc, char *argv[])
{
    // if author command, print the author
    if (argc == 1 && strcmp(argv[0], "author") == 0)
    {
        printf("Niyomwungeri Parmenide Parmenide\n");
        return 0;
    }

    // author command failed
    return 1;
}

/*
 * Handles the cd, by setting cwd to argv[1], which must exist.
 *
 * Parameters:
 *   argc     The length of the argv vector
 *   argv[]   The argument vector, which must have either 1 or 2 arguments.
 *
 * Returns:
 *   0 on success, 1 on failure
 */
static int fn_cd(int argc, char *argv[])
{
    // if the first arg is cd COMMAND
    if (argc == 2 && strcmp(argv[0], "cd") == 0)
    {
        // Go to the directory specified by argv[1]
        if (chdir(argv[1]) == 0)
            return 0;
    }

    // cd command failed
    return 1;
}

/*
 * Handles the pwd, by printing the cwd to the supplied file
 * descriptor
 *
 * Parameters (which are all ignored):
 *   argc     The length of the argv vector
 *   argv[]   The argument vector
 *
 * Returns:
 *   Always returns 0, since it always succeeds
 */
static int fn_pwd(int argc, char *argv[])
{
    // if the first arg is pwd COMMAND
    if (argc == 1 && strcmp(argv[0], "pwd") == 0)
    {
        char currentDir[1024];
        getcwd(currentDir, sizeof(currentDir));

        // print the current directory
        printf("%s \n", currentDir);
        return 0;
    }

    // pwd command failed
    return 1;
}

/*
 * Process an external command, by forking and executing
 * a child process, and waiting for the child to terminate
 *
 * Parameters:
 *   argc      The length of the argv vector
 *   argv[]    The argument vector
 *
 * Returns:
 *   The child's exit value, or -1 on error
 */
static int forkexec_external_cmd(int argc, char *argv[])
{
    // start a child process to execute the command
    pid_t pid = fork();

    // if the child process, execute the command
    if (pid == 0)
    {
        execvp(argv[0], argv);
        exit(0);
    }

    // if the parent process, wait for the child to terminate
    else if (pid > 0)
    {
        int status;
        waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
    }
    // fork failed
    else
    {
        fprintf(stderr, "Child %d exited with status %d \n", pid, WEXITSTATUS(pid));
        return -1;
    }
}

/*
 * Parses one input line, and executes it
 *
 * Parameters:
 *   argc     The length of the argv vector, which must be >= 1
 *   argv[]   The argument vector
 */
void execute_command(int argc, char *argv[])
{
    assert(argc >= 1);
    printf("argc: %d \n", argc);
    printf("argv[0]: %s \n", argv[0]);

    // If exit or quit command
    if (strcmp(argv[0], "exit") == 0 || strcmp(argv[0], "quit") == 0)
        fn_exit(argc, argv);

    // If author command
    else if (strcmp(argv[0], "author") == 0)
        fn_author(argc, argv);

    // If cd command
    else if (strcmp(argv[0], "cd") == 0)
        fn_cd(argc, argv);

    // If pwd command
    else if (strcmp(argv[0], "pwd") == 0)
        fn_pwd(argc, argv);

    // If command is not built-in, execute it as a child process
    else
        forkexec_external_cmd(argc, argv);
}


int main(int argc, char *argv[])
{
    char *input = NULL;

    fprintf(stdout, "Welcome to Plaid Shell!\n");
    const char *prompt = "#> ";

    // main loop of the shell - read, parse, execute commands
    while (1)
    {
        // get the input from the user
        input = readline(prompt);

        // store the input in the history buffer
        add_history(input);

        // if the user entered nothing, go back to the prompt
        if (input == NULL)
            exit(0);

        if (*input == '\0')
            continue;

        // parse the input into arguments
        // argc = parse_input(input, argv, MAX_ARGS);
        argc = 1;
        argv[0] = input;

        // if the input was empty, go back to the prompt
        if (argc == -1)
            printf(" Error: %s\n", argv[0]);

        // if the input was not empty, execute the command
        else
            for (int i = 0; i < argc; i++)
                execute_command(argc, argv);
    }

    return 0;
}