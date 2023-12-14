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
 *  errmsg: the error message buffer
 *  errmsg_sz: the size of the error message buffer
 *
 * Returns:
 *  a pointer to the pipeline data structure
 */
pipeline_t *parse_tokens(CList list, char *errmsg, size_t errmsg_sz);

#endif