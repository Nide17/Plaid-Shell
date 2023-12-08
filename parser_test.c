/**
 * Tests for the parser function
 *
 * Contributor: Niyomwungeri Parmenide Ishimwe <parmenin@andrew.cmu.edu>
 */

#include <stdio.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "parser.h"
#include "pipeline.h"
#include "token.h"
#include "clist.h"
#include "tokenize.h"

// If value is not true; prints a failure message and returns 0.
#define test_assert(value)                                               \
    {                                                                    \
        if (!(value))                                                    \
        {                                                                \
            printf("FAIL %s[%d]: %s\n", __FUNCTION__, __LINE__, #value); \
            goto test_error;                                             \
        }                                                                \
    }

/*
 * Exactly like strcmp, but ignores spaces.  Therefore the following
 * strings compare alike: "ab", " ab", "  a  b  ", "a b"
 *
 * Parameters:
 *   s1, s2    The strings to be compared
 *
 * Returns: -1, 0, or 1 depending on whether s1 sorts before, equal
 * to, or after s2.
 */
static int strcmp_sp(const char *s1, const char *s2)
{
    // advance past leading spaces
    while (isblank(*s1))
        s1++;
    while (isblank(*s2))
        s2++;

    while (*s1 != '\0')
    {
        if (isblank(*s1))
            s1++;
        else if (isblank(*s2))
            s2++;
        else if (*s1 != *s2)
            return (*s1 - *s2);
        else
        {
            s1++;
            s2++;
        }
    }

    while (isblank(*s2))
        s2++;

    return (*s1 - *s2);
}

/*
 * Test the parser function with a single pipe token
 *
 * Parameters:
 *   None
 *
 * Returns:
 *   1 if the test passed, 0 otherwise
 */
int test_parse_tokens_pipe_token()
{
    // Create a list of tokens with a single pipe token
    char errmsg[128];
    CList tokens = NULL;
    pipeline_t *pipeline = NULL;

    tokens = TOK_tokenize_input("pwd", errmsg, sizeof(errmsg));
    test_assert(tokens != NULL);
    test_assert(tokens->length == 1);
    test_assert(CL_nth(tokens, 0).type == TOK_WORD);
    test_assert(strcmp(CL_nth(tokens, 0).text, "pwd") == 0);

    // Check that the pipeline object has a single node
    pipeline = parse_tokens(tokens);
    test_assert(pipeline->head != NULL);
    test_assert(pipeline->head->next == NULL);

    // Check input and output
    test_assert(pipeline->input == NULL);
    test_assert(pipeline->output == NULL);

    // Check that the node has a single command
    test_assert(pipeline->head->args[0] != NULL);
    test_assert(strcmp_sp(pipeline->head->args[0], "pwd") == 0);
    test_assert(pipeline->head->args[1] == NULL);

    // check the length of the pipeline
    test_assert(pipeline->length == 1);
    pipeline_free(pipeline);
    CL_free(tokens);

    tokens = TOK_tokenize_input("echo a b", errmsg, sizeof(errmsg));
    pipeline = parse_tokens(tokens);
    test_assert(CL_length(tokens) == 3);
    test_assert(TOK_next_type(tokens) == TOK_WORD);
    test_assert(strcmp_sp(TOK_next(tokens).text, "echo") == 0);

    // Check that the pipeline object has a single node
    test_assert(pipeline->head != NULL);
    test_assert(pipeline->head->next == NULL);

    // Check input and output
    test_assert(pipeline->input == NULL);
    test_assert(pipeline->output == NULL);

    // Check that the node has a single command
    test_assert(pipeline->head->args[0] != NULL);
    test_assert(strcmp_sp(pipeline->head->args[0], "echo") == 0);
    test_assert(strcmp_sp(pipeline->head->args[1], "a") == 0);
    test_assert(strcmp_sp(pipeline->head->args[2], "b") == 0);
    test_assert(pipeline->head->args[3] == NULL);
    test_assert(pipeline->length == 1);

    // Free the pipeline object and the list of tokens
    pipeline_free(pipeline);
    CL_free(tokens);

    tokens = TOK_tokenize_input("echo a b | grep c", errmsg, sizeof(errmsg));
    pipeline = parse_tokens(tokens);
    test_assert(CL_length(tokens) == 5);
    test_assert(TOK_next_type(tokens) == TOK_WORD);
    test_assert(strcmp_sp(TOK_next(tokens).text, "echo") == 0);

    // Check that the pipeline object has a single node
    test_assert(pipeline->head != NULL);
    test_assert(pipeline->head->next != NULL);
    test_assert(pipeline->head->next->next == NULL);

    // Check input and output
    test_assert(pipeline->input == NULL);
    test_assert(pipeline->output == NULL);

    // Check that the node has a single command
    test_assert(pipeline->head->args[0] != NULL);
    test_assert(strcmp_sp(pipeline->head->args[0], "echo") == 0);
    test_assert(strcmp_sp(pipeline->head->args[1], "a") == 0);
    test_assert(strcmp_sp(pipeline->head->args[2], "b") == 0);
    test_assert(pipeline->head->args[3] == NULL);

    // Check that the node has a single command
    test_assert(pipeline->head->next->args[0] != NULL);
    test_assert(strcmp_sp(pipeline->head->next->args[0], "grep") == 0);
    test_assert(strcmp_sp(pipeline->head->next->args[1], "c") == 0);
    test_assert(pipeline->head->next->args[2] == NULL);

    // check the length of the pipeline
    test_assert(pipeline->length == 2);
    pipeline_free(pipeline);
    CL_free(tokens);

    return 1;

test_error:
    CL_free(tokens);
    pipeline_free(pipeline);
    return 0;
}

int main()
{
    int passed = 0;
    int num_tests = 0;

    num_tests++;
    passed += test_parse_tokens_pipe_token();

    printf("Passed %d/%d test cases\n", passed, num_tests);
    fflush(stdout);
    return 0;
}