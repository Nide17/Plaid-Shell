// Create a parser that accepts the list of tokens as an input, and outputs an AST (the Pipeline data structure).

// The parser will be simpler than the recursive descent parser we implemented for ExprWhizz; mine is a simple loop with no recursion
// The parser will be a function that takes a list of tokens as an input, and outputs a pipeline data structure
/*
 * parser.h
 *
 * Functions for parsing the tokens into a pipeline data structure
 *
 * Author: Niyomwungeri Parmenide Ishimwe <parmenin@andrew.cmu.edu>
 */

#ifndef PARSER_H
#define PARSER_H

#include "pipeline.h"
#include "clist.h"

/**
 * Parses the tokens into a pipeline data structure
 *
 * Parameters:
 *  tokens: the list of tokens to parse
 *
 * Returns:
 *  a pointer to the pipeline data structure
 */
pipeline_t *parse_tokens(CList list);

#endif