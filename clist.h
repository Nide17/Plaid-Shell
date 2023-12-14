/*
 * clist.h
 *
 * Linked list implementation for ISSE Assignment 12
 *
 * Author: Niyomwungeri Parmenide Ishimwe <parmenin@andrew.cmu.edu>
 *
 */
#ifndef _CLIST_H_
#define _CLIST_H_

#include <stdbool.h>
#include "token.h"

struct _cl_node
{
    Token tok_elt;
    struct _cl_node *next;
};

struct _clist
{
    struct _cl_node *head;
    int length;
};

// struct _clist to be used in the .c as CList
typedef struct _clist *CList;

// Indicates an error: { TOK_WORD, NULL };
#define EMPTY_TOKEN \
    (Token) { .type = TOK_WORD, .text = NULL }

/*
 * Create a new CList
 *
 * Parameters: None
 *
 * Returns: The new list
 */
CList CL_new();

/*
 * Destroy a list, calling free() on all malloc'd memory.
 *
 * Parameters:
 *   list   The list
 *
 * Returns: None
 */
void CL_free(CList list);

/*
 * Compute the length of a list
 *
 * Parameters:
 *   list   The list
 *
 * Returns: The length of the list, or 0 if list is empty
 */
int CL_length(CList list);

/*
 * Append the specfied element to the tail of the list
 *
 * Parameters:
 *   list     The list
 *   element  The element to append
 *
 * Returns: None
 */
void CL_append(CList list, Token element);

/*
 * Return the Nth element, without modifying the list
 *
 * Parameters:
 *   list     The list
 *   pos      Position to return
 *
 * If pos >= 0, the corresponding element will be returned, counting 0
 * as the head element.  So pos == 0 will return the head element, and
 * pos == 1 will return the second element on the list.
 *
 * If pos <= -1, the corresponding element counting from the end of
 * the list will be returned, so for instance pos == -1 will return the
 * tail element and pos == -2 will return the element before the tail
 * element.
 *
 * pos must be in the range [-length, length-1] inclusive. If pos is
 * outside this range, returns INVALID_RETURN.
 *
 * Returns: The requested element, or INVALID_RETURN if no element was found.
 */
Token CL_nth(CList list, int pos);

/*
 * Remove an element from the specified position and return it.
 *
 * Parameters:
 *   list     The list
 *   pos      Position to perform the removal
 *
 * If pos >= 0, the corresponding element will be removed, counting 0
 * as the head element.  So pos == 0 will remove and return the head
 * element, and pos == 1 will remove the second element on the list.
 *
 * If pos <= -1, the corresponding element counting from the end of
 * the list will be removed and returned, so for instance pos == -1
 * will remove the tail element and pos == -2 will remove the element
 * before the tail element.
 *
 * pos must be in the range [-length, length-1] inclusive. If pos is
 * outside this range, returns INVALID_RETURN.
 *
 * Returns: The element that was removed, or INVALID_RETURN if no
 *   element was removed.
 */
Token CL_remove(CList list, int pos);

/*
 * Remove the element from the head of the list and return
 * it. If the list is empty, return INVALID_RETURN.
 *
 * Parameters:
 *   list     The list
 *
 * Returns: The popped item
 */
Token CL_pop(CList list);

typedef void (*CL_foreach_callback)(int pos, Token element, void *cb_data);
/*
 * Iterate through the list; call the user-specified callback function
 * for each element.  Each call to callback will be of the form
 *
 *   callback( <element's position>, <element>, <cb_data> )
 *
 * Parameters:
 *   list       The list
 *   callback   The function to call
 *   cb_data    Caller data to pass to the function
 *
 * Returns: None
 */
void CL_foreach(CList list, CL_foreach_callback callback, void *cb_data);

#endif /* _CLIST_H_ */
