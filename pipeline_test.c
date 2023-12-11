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
    pipeline_add_command(pipeline, "pwd");
    pipeline_add_command(pipeline, "ls");
    assert(pipeline->length == 2);
    assert(strcmp(pipeline_get_command(pipeline, 0), "pwd") == 0);
    assert(strcmp(pipeline_get_command(pipeline, 1), "ls") == 0);

    // Get Command by Index: "pwd", "ls"
    assert(strcmp(pipeline_get_command(pipeline, 0), "pwd") == 0);
    assert(strcmp(pipeline_get_command(pipeline, 1), "ls") == 0);

    // Free Pipeline
    pipeline_free(pipeline);

    // Edge Cases
    pipeline = pipeline_new();
    char long_command[256] = "a";
    for (int i = 0; i < MAX_ARGS; i++)
    {
        strcat(long_command, "a");
        pipeline_add_command(pipeline, long_command);
    }

    assert(pipeline->length == MAX_ARGS);
    assert(pipeline_get_command(pipeline, 0) != NULL);

    pipeline_free(pipeline);
}

int main()
{
    run_test_cases();
    printf("All passed!\n");
    return 0;
}
