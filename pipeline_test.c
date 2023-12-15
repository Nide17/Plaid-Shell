#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "pipeline.h"

void run_test_cases()
{
    // Create a Pipeline
    pipeline_t *pipeline = pipeline_new();
    assert(pipeline != NULL);
    assert(pipeline->length == 0);
    assert(pipeline_get_input(pipeline) == NULL);
    assert(pipeline_get_output(pipeline) == NULL);

    // Set and Get Input/Output Files
    pipeline_set_input(pipeline, "input_file.txt");
    pipeline_set_output(pipeline, "output_file.txt");
    assert(strcmp(pipeline_get_input(pipeline), "input_file.txt") == 0);
    assert(strcmp(pipeline_get_output(pipeline), "output_file.txt") == 0);

    // Add Commands to Pipeline
    pipeline_cmd_t *pwd_node = pipeline_cmd_new(TOK_WORD);
    pipeline_cmd_add_arg(pwd_node, "pwd");
    pipeline_add_command(pipeline, pwd_node);

    pipeline_cmd_t *ls_node = pipeline_cmd_new(TOK_WORD);
    pipeline_cmd_add_arg(ls_node, "ls");
    pipeline_add_command(pipeline, ls_node);

    assert(pipeline->length == 2);
    assert(strcmp(pipeline_get_command(pipeline, 0)->args[0], "pwd") == 0);
    assert(strcmp(pipeline_get_command(pipeline, 1)->args[0], "ls") == 0);

    // Free Pipeline
    pipeline_free(pipeline);

    // Edge Cases
    pipeline = pipeline_new();
    char long_command[256] = "a";
    for (int i = 0; i < MAX_ARGS; i++)
    {
        strcat(long_command, "a");
        pipeline_cmd_t *node = pipeline_cmd_new(TOK_WORD);
        pipeline_cmd_add_arg(node, long_command);
        pipeline_add_command(pipeline, node);
    }

    assert(pipeline->length == MAX_ARGS);
    assert(pipeline_get_command(pipeline, 0) != NULL);
    pipeline_free(pipeline);

    // More commands
    pipeline = pipeline_new();
    pipeline_set_input(pipeline, "input_file.txt");
    pipeline_set_output(pipeline, "output_file.txt");

    pipeline_cmd_t *cat_node = pipeline_cmd_new(TOK_WORD);
    pipeline_cmd_add_arg(cat_node, "cat");
    pipeline_add_command(pipeline, cat_node);

    pipeline_cmd_t *grep_node = pipeline_cmd_new(TOK_WORD);
    pipeline_cmd_add_arg(grep_node, "grep");
    pipeline_cmd_add_arg(grep_node, "hello");
    pipeline_add_command(pipeline, grep_node);

    pipeline_cmd_t *wc_node = pipeline_cmd_new(TOK_WORD);
    pipeline_cmd_add_arg(wc_node, "wc");
    pipeline_add_command(pipeline, wc_node);

    // Free Pipeline
    pipeline_free(pipeline);

    // Pipe, and redirection nodes
    pipeline = pipeline_new();
    pipeline_set_input(pipeline, "input_file.txt");
    pipeline_set_output(pipeline, "output_file.txt");

    // Get Input/Output Files
    assert(strcmp(pipeline_get_input(pipeline), "input_file.txt") == 0);
    assert(strcmp(pipeline_get_output(pipeline), "output_file.txt") == 0);

    pipeline_cmd_t *cat_node2 = pipeline_cmd_new(TOK_WORD);
    pipeline_cmd_add_arg(cat_node2, "cat");
    pipeline_add_command(pipeline, cat_node2);

    // Get Command by Index: "cat"
    assert(strcmp(pipeline_get_command(pipeline, 0)->args[0], "cat") == 0);

    pipeline_cmd_t *grep_node2 = pipeline_cmd_new(TOK_WORD);
    pipeline_cmd_add_arg(grep_node2, "grep");
    pipeline_cmd_add_arg(grep_node2, "hello");
    pipeline_add_command(pipeline, grep_node2);

    // Get Command by Index: "cat", "grep"
    assert(strcmp(pipeline_get_command(pipeline, 0)->args[0], "cat") == 0);
    assert(strcmp(pipeline_get_command(pipeline, 1)->args[0], "grep") == 0);

    // grep node args
    pipeline_cmd_t *node1 = pipeline->head;
    assert(strcmp(node1->args[0], "cat") == 0);

    pipeline_cmd_t *node2 = node1->next;
    assert(strcmp(node2->args[0], "grep") == 0);
    assert(strcmp(node2->args[1], "hello") == 0);

    pipeline_cmd_t *wc_node2 = pipeline_cmd_new(TOK_WORD);
    pipeline_cmd_add_arg(wc_node2, "wc");
    pipeline_add_command(pipeline, wc_node2);

    // Get Command by Index: "cat", "grep", "wc"
    assert(strcmp(pipeline_get_command(pipeline, 0)->args[0], "cat") == 0);
    assert(strcmp(pipeline_get_command(pipeline, 1)->args[0], "grep") == 0);
    assert(strcmp(pipeline_get_command(pipeline, 2)->args[0], "wc") == 0);

    pipeline_cmd_t *pipe_node = pipeline_cmd_new(TOK_PIPE);
    pipeline_add_command(pipeline, pipe_node);

    // Get Command by Index: "cat", "grep", "wc", "|"
    assert(strcmp(pipeline_get_command(pipeline, 0)->args[0], "cat") == 0);
    assert(strcmp(pipeline_get_command(pipeline, 1)->args[0], "grep") == 0);
    assert(strcmp(pipeline_get_command(pipeline, 2)->args[0], "wc") == 0);
    assert(pipeline_get_command(pipeline, 3)->type == TOK_PIPE);

    pipeline_cmd_t *cat_node3 = pipeline_cmd_new(TOK_WORD);
    pipeline_cmd_add_arg(cat_node3, "cat");
    pipeline_add_command(pipeline, cat_node3);

    pipeline_cmd_t *grep_node3 = pipeline_cmd_new(TOK_WORD);
    pipeline_cmd_add_arg(grep_node3, "grep");
    pipeline_cmd_add_arg(grep_node3, "hello");
    pipeline_add_command(pipeline, grep_node3);

    pipeline_cmd_t *wc_node3 = pipeline_cmd_new(TOK_WORD);
    pipeline_cmd_add_arg(wc_node3, "wc");
    pipeline_add_command(pipeline, wc_node3);

    pipeline_cmd_t *redir_node = pipeline_cmd_new(TOK_GREATERTHAN);
    pipeline_add_command(pipeline, redir_node);

    pipeline_cmd_t *cat_node4 = pipeline_cmd_new(TOK_WORD);
    pipeline_cmd_add_arg(cat_node4, "cat");
    pipeline_add_command(pipeline, cat_node4);

    pipeline_cmd_t *grep_node4 = pipeline_cmd_new(TOK_WORD);
    pipeline_cmd_add_arg(grep_node4, "grep");
    pipeline_cmd_add_arg(grep_node4, "hello");
    pipeline_add_command(pipeline, grep_node4);

    pipeline_cmd_t *wc_node4 = pipeline_cmd_new(TOK_WORD);
    pipeline_cmd_add_arg(wc_node4, "wc");
    pipeline_add_command(pipeline, wc_node4);

    // Free Pipeline
    pipeline_free(pipeline);
}

int main()
{
    run_test_cases();
    printf("All tests passed!\n");
    return 0;
}
