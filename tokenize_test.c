/**
 * tokenize_test.c
 *
 * This file contains the test cases for tokenize.c
 */

#include <stdio.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "clist.h"
#include "tokenize.h"
#include "token.h"

// If value is not true; prints a failure message and returns 0.
#define test_assert(value)                                               \
    {                                                                    \
        if (!(value))                                                    \
        {                                                                \
            printf("FAIL %s[%d]: %s\n", __FUNCTION__, __LINE__, #value); \
            goto test_error;                                             \
        }                                                                \
    }

// TOK_WORD,
// TOK_QUOTED_WORD,
// TOK_LESSTHAN,
// TOK_GREATERTHAN,
// TOK_PIPE,
// TOK_END
Token tokens[] = {{TOK_WORD, "ls"}, {TOK_WORD, "-l"}, {TOK_WORD, "-a"}, {TOK_WORD, "foo"}, {TOK_WORD, "bar"}, {TOK_WORD, "baz"}, {TOK_WORD, "qux"}, {TOK_QUOTED_WORD, "foo bar baz"}, {TOK_LESSTHAN, "<"}, {TOK_WORD, "input.txt"}, {TOK_GREATERTHAN, ">"}, {TOK_WORD, "output.txt"}, {TOK_PIPE, "|"}, {TOK_WORD, "wc"}, {TOK_WORD, "-l"}, {TOK_END, NULL}};

const int num_tokens = sizeof(tokens) / sizeof(tokens[0]);

/*
 * Returns true if tok1 and tok2 compare equally, false otherwise
 */
static bool test_tok_eq(Token tok1, Token tok2)
{
    if (tok1.type != tok2.type)
        return false;

    return true;
}

struct inspect_data
{
    int num_calls;
    int num_correct;
};

void inspect_element(int pos, Token element, void *data)
{
    struct inspect_data *id = (struct inspect_data *)data;

    id->num_calls++;
    if (test_tok_eq(element, tokens[pos]))
        id->num_correct++;
}

/*
 * Tests the CL_new, CL_free, CL_append, CL_length, CL_nth, CL_foreach
 * and CL_pop functions, using struct token type. This should give you
 * some assurance that your CList is working correctly.
 *
 * Returns: 1 if all tests pass, 0 otherwise
 */
int test_cl_token()
{
    struct inspect_data id = {0, 0};
    CList list = CL_new();

    for (int i = 0; i < num_tokens; i++)
    {
        CL_append(list, tokens[i]);
        test_assert(CL_length(list) == i + 1);
        test_assert(test_tok_eq(CL_nth(list, i), tokens[i]));
    }

    // test foreach
    CL_foreach(list, inspect_element, &id);
    test_assert(id.num_calls == num_tokens);
    test_assert(id.num_correct == num_tokens);

    // pop everything off, make sure that works
    for (int i = 0; i < num_tokens; i++)
        test_assert(test_tok_eq(CL_pop(list), tokens[i]));

    test_assert(CL_length(list) == 0);

    CL_free(list);
    return 1;

test_error:
    CL_free(list);
    return 0;
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
 * Tests the TOK_next_type and TOK_consume functions
 *
 * Returns: 1 if all tests pass, 0 otherwise
 */
int test_tok_next_consume()
{
    CList list = CL_new();

    for (int i = 0; i < num_tokens; i++)
    {
        CL_append(list, tokens[i]);
        test_assert(CL_length(list) == i + 1);
        test_assert(test_tok_eq(CL_nth(list, i), tokens[i]));
    }

    for (int i = 0; i < num_tokens; i++)
    {
        test_assert(TOK_next_type(list) == tokens[i].type);
        TOK_consume(list);
    }

    test_assert(CL_length(list) == 0);
    test_assert(TOK_next_type(list) == TOK_END);
    TOK_consume(list);
    test_assert(TOK_next_type(list) == TOK_END);
    TOK_consume(list);
    test_assert(TOK_next_type(list) == TOK_END);
    TOK_consume(list);

    CL_free(list);
    return 1;

test_error:
    CL_free(list);
    return 0;
}

/*
 * Tests the TOK_tokenize_input function
 *
 * Returns: 1 if all tests pass, 0 otherwise
 */
int test_tokenize_input()
{
    char errmsg[128];
    CList list = NULL;

    list = TOK_tokenize_input("pwd", errmsg, sizeof(errmsg));
    test_assert(CL_length(list) == 1);
    test_assert(TOK_next_type(list) == TOK_WORD);
    test_assert(strcmp_sp(TOK_next(list).text, "pwd") == 0);
    CL_free(list);

    list = TOK_tokenize_input("   pwd   ", errmsg, sizeof(errmsg));
    test_assert(CL_length(list) == 1);
    test_assert(TOK_next_type(list) == TOK_WORD);
    test_assert(strcmp_sp(TOK_next(list).text, "pwd") == 0);
    CL_free(list);

    list = TOK_tokenize_input("echo a b", errmsg, sizeof(errmsg));
    test_assert(CL_length(list) == 3);
    test_assert(TOK_next_type(list) == TOK_WORD);
    test_assert(strcmp_sp(TOK_next(list).text, "echo") == 0);
    CL_free(list);

    // echo a\ b
    list = TOK_tokenize_input("echo a\\ b", errmsg, sizeof(errmsg));
    test_assert(CL_length(list) == 2);
    test_assert(TOK_next_type(list) == TOK_WORD);
    test_assert(strcmp_sp(TOK_next(list).text, "echo") == 0);
    CL_free(list);

    // echo "a b"
    list = TOK_tokenize_input("echo \"a b\"", errmsg, sizeof(errmsg));
    test_assert(CL_length(list) == 2);
    test_assert(TOK_next_type(list) == TOK_WORD);
    CL_free(list);

    // echo a\\ b
    list = TOK_tokenize_input("echo a\\\\ b", errmsg, sizeof(errmsg));
    test_assert(CL_length(list) == 3);
    test_assert(TOK_next_type(list) == TOK_WORD);
    test_assert(strcmp_sp(TOK_next(list).text, "echo") == 0);
    CL_free(list);

    // echo hello|grep "ell" => WORD “echo”; WORD “hello”; PIPE; WORD “grep”; QUOTED_WORD “ell”
    list = TOK_tokenize_input("echo hello|grep \"ell\"", errmsg, sizeof(errmsg));
    test_assert(CL_length(list) == 5);
    test_assert(TOK_next_type(list) == TOK_WORD);
    test_assert(strcmp_sp(TOK_next(list).text, "echo") == 0);
    CL_free(list);

    // echo boo >out_file => WORD “echo”; WORD “boo”; GREATERTHAN; WORD “out_file”
    list = TOK_tokenize_input("echo boo >out_file", errmsg, sizeof(errmsg));
    test_assert(CL_length(list) == 4);
    test_assert(TOK_next_type(list) == TOK_WORD);
    test_assert(strcmp_sp(TOK_next(list).text, "echo") == 0);
    CL_free(list);

    // echo"boo">out_file => WORD “echo”; QUOTED_WORD “boo”; GREATERTHAN; WORD “out_file”
    list = TOK_tokenize_input("echo\"boo\">out_file", errmsg, sizeof(errmsg));
    test_assert(CL_length(list) == 4);
    test_assert(TOK_next_type(list) == TOK_WORD);
    test_assert(strcmp_sp(TOK_next(list).text, "echo") == 0);
    CL_free(list);

    // (no input)
    list = TOK_tokenize_input("", errmsg, sizeof(errmsg));
    test_assert(CL_length(list) == 0);
    CL_free(list);

    // echo "hello | grep"
    list 

    return 1;

test_error:
    CL_free(list);
    return 0;
}

int main()
{
    int passed = 0;
    int num_tests = 0;

    num_tests++;
    passed += test_cl_token();
    num_tests++;
    passed += test_tok_next_consume();
    num_tests++;
    passed += test_tokenize_input();

    printf("Passed %d/%d test cases\n", passed, num_tests);
    fflush(stdout);
    return 0;
}