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

    list = TOK_tokenize_input("ls -l -a foo bar baz qux", errmsg, sizeof(errmsg));
    test_assert(CL_length(list) == 7);
    test_assert(TOK_next_type(list) == TOK_WORD);
    test_assert(strcmp_sp(TOK_next(list).text, "ls") == 0);
    CL_free(list);

    list = TOK_tokenize_input("ls -l -a \"foo bar baz\" < input.txt > output.txt | wc -l", errmsg, sizeof(errmsg));
    test_assert(CL_length(list) == 13);
    test_assert(TOK_next_type(list) == TOK_WORD);
    test_assert(strcmp_sp(TOK_next(list).text, "ls") == 0);
    CL_free(list);

    /**
    * $ ./plaidsh
    Welcome to Plaid Shell!
    #? pwd
    /home/howdy/Teaching/CMU-ISSE/Assignments/12
    #? ls --color
    'CMU plaid.pxd' pipeline.c setup_playground.sh
    'ISSE Assignment 12.docx' pipeline.h test.psh
    Makefile pipeline.o token.h
    clist.c plaidsh tokenize.c
    clist.h plaidsh.c tokenize.h
    clist.o plaidsh.o tokenize.o
    parse.c ps_test '~$SE Assignment 12.docx'
    parse.h ps_test.c
    parse.o ps_test.o
    #? ./setup_playground.sh
    Plaid Shell playground created...
    #? cd Plaid\ Shell\ Playground
    #? ls
    README 'best sitcoms.txt' 'seven dwarfs.txt' shells.txt
    #? ls *.txt
    'best sitcoms.txt' 'seven dwarfs.txt' shells.txt
    #? echo $PATH
    $PATH
    #? author
    Howdy Pierce
    #? author | sed -e "s/^/Written by /"
    Written by Howdy Pierce
    #? grep Happy *.txt
    best sitcoms.txt:Happy Days
    seven dwarfs.txt:Happy
    #? cat "best sitcoms.txt" | grep Seinfeld
    Seinfeld
    #? sed -ne "s/The Simpsons/I Love Lucy/p" < best\ sitcoms.txt > output
    #? ls -l
    total 20
    -rw-rw---- 1 howdy howdy 64 Nov 15 01:48 README
    -rw-rw---- 1 howdy howdy 142 Nov 15 01:48 'best sitcoms.txt'
    -rw-rw---- 1 howdy howdy 12 Nov 15 01:48 output
    ISSE Final Project: Plaid Shell Due: Thursday, Dec. 14, 5:00 pm CAT
    Howdy Pierce Introduction to System Software Engineering Page 2
    -rw-rw---- 1 howdy howdy 45 Nov 15 01:48 'seven dwarfs.txt'
    -rw-rw---- 1 howdy howdy 25 Nov 15 01:48 shells.txt
    #? cat output
    I Love Lucy
    #? this is not a command
    this: Command not found
    Child 46881 exited with status 2
    #? echo Hello > /usr/bin/cant_write
    /usr/bin/cant_write: Permission denied
    #? cd
    #? pwd
    /home/howdy
    #? cd ~
    #? pwd
    /home/howdy
    #? exit
    */

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