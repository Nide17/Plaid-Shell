/*
 * clist.c
 *
 * Linked list implementation for ISSE Assignment 11
 *
 * Author: Niyomwungeri Parmenide Ishimwe <parmenin@andrew.cmu.edu>
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "clist.h"

#define DEBUG

/*
 * Create (malloc) a new _cl_node and populate it with the supplied values
 *
 * Parameters:
 *   element, next  The values for the node to be created
 *
 * Returns: The newly-malloc'd node, or NULL in case of error
 */
static struct _cl_node *_CL_new_node(Token tok, struct _cl_node *next)
{
  struct _cl_node *new = (struct _cl_node *)malloc(sizeof(struct _cl_node));
  assert(new);

  new->tok_elt = tok;
  new->next = next;

  return new;
}

// Documented in .h file
CList CL_new()
{
  CList list = (CList)malloc(sizeof(struct _clist));
  assert(list != NULL);

  list->head = NULL;
  list->length = 0;

  return list;
}

// Documented in .h file
void CL_free(CList list)
{
  if (list == NULL)
    return;

  // deallocate all the nodes in the list
  struct _cl_node *this_node = list->head;

  if (this_node == NULL)
  {
    free(list);
    list = NULL;
    return;
  }

  // traverse the list, deallocating each node
  while (this_node != NULL)
  {
    // save a pointer to the next node
    struct _cl_node *next_node = this_node->next;

    // deallocate the element of the current node if exists
    if (this_node->tok_elt.type == TOK_WORD || this_node->tok_elt.type == TOK_QUOTED_WORD)
    {
      if (this_node->tok_elt.text != NULL)
      {
        free(this_node->tok_elt.text);
        this_node->tok_elt.text = NULL;
      }
    }

    // deallocate the current node
    free(this_node);
    this_node = NULL;

    // move on to the next node
    this_node = next_node;
  }

  // deallocate the list structure itself
  free(list);
  list = NULL;
}

// Documented in .h file
int CL_length(CList list)
{
  if (list == NULL)
    return 0;

    // traverse the list, counting the number of nodes
#ifdef DEBUG
  int len = 0;

  for (struct _cl_node *node = list->head; node != NULL; node = node->next)
    len++;

  assert(len == list->length);
#endif // DEBUG

  return list->length;
}

// Documented in .h file
void CL_append(CList list, Token tok)
{
  int non_space = 0;
  // check if the token word or quoted word and it is empty or spaces only
  if (tok.type == TOK_WORD || tok.type == TOK_QUOTED_WORD)
  {
    for (int i = 0; i < strlen(tok.text); i++)
    {
      if (!isspace(tok.text[i]))
      {
        non_space = 1;
        break;
      }
    }
  }

  // if the token is empty or spaces only, do nothing
  if ((tok.type == TOK_WORD || tok.type == TOK_QUOTED_WORD) && non_space == 0)
    return;

  if (list == NULL)
    return;

  // new node to append - its next pointer should be NULL
  struct _cl_node *new_node = _CL_new_node(tok, NULL);

  // when appending to an empty list, the new node becomes the head
  if (list->head == NULL)
    list->head = new_node;

  else
  {
    // otherwise, traverse the list until we find the last node
    struct _cl_node *this_node = list->head;
    while (this_node->next != NULL)
      this_node = this_node->next;

    // then append the new node to the end of the list
    this_node->next = new_node;
  }

  // increment the length of the list
  list->length++;
}

// Documented in .h file
Token CL_nth(CList list, int pos)
{
  if (list == NULL)
    return EMPTY_TOKEN;

  // bounds check - if pos is negative or out of bounds, it's an error
  if (pos < -list->length || pos >= list->length)
    return EMPTY_TOKEN;

  // convert negative pos to positive by counting from the end of the list
  if (pos < 0)
    pos = list->length + pos;

  // traverse the list until we find the node at position pos
  struct _cl_node *this_node = list->head;
  while (pos > 0 && this_node != NULL)
  {
    this_node = this_node->next;
    pos--;
  }

  // if this_node is NULL, we are at the end of the list
  if (this_node == NULL)
    return EMPTY_TOKEN;

  // otherwise, this_node points to the node at position pos
  return this_node->tok_elt;
}
// Documented in .h file
Token CL_remove(CList list, int pos)
{
  if (list == NULL)
    return EMPTY_TOKEN;

  // If pos is negative, count from the end of the list
  if (pos < 0)
    pos = list->length + pos;

  // If pos is still negative or out of bounds, it's an error
  if (pos < 0 || pos >= list->length)
    return EMPTY_TOKEN;

  // If pos is 0, just pop the head of the list
  if (pos == 0)
    return CL_pop(list);

  // traverse the list until we find the node at position pos-1
  struct _cl_node *this_node = list->head;
  while (pos > 1 && this_node != NULL)
  {
    this_node = this_node->next;
    pos--;
  }

  // If this_node is NULL, we are at the end of the list
  if (this_node == NULL)
    return EMPTY_TOKEN;

  // if this_node->next is NULL, we are at the end of the list
  struct _cl_node *rm_node = this_node->next;
  if (rm_node == NULL)
    return EMPTY_TOKEN;

  else
  {
    // remove the node at position pos-1 - point current node to the node after the one we are removing
    this_node->next = rm_node->next;

    // Save the element to return
    Token rm_element = rm_node->tok_elt;

    // Decrement the length of the list
    list->length--;

    // deallocate the node we are removing
    free(rm_node);

    return rm_element;
  }
}

// Documented in .h file
Token CL_pop(CList list)
{
  if (list == NULL)
    return EMPTY_TOKEN;

  struct _cl_node *popped_node = list->head;

  if (popped_node == NULL)
    return EMPTY_TOKEN;

  Token ret = popped_node->tok_elt;

  // unlink previous head node, then free it
  list->head = popped_node->next;
  free(popped_node);

  list->length--;

  return ret;
}

// Documented in .h file
void CL_foreach(CList list, CL_foreach_callback callback, void *cb_data)
{
  if (list == NULL)
    return;

  // if list is empty, or callback is NULL, or cb_data is NULL, do nothing
  if (callback == NULL || list->head == NULL || cb_data == NULL)
    return;

  // traverse the list, calling the callback function for each element if it is not NULL
  int position = 0;
  for (struct _cl_node *this_node = list->head; this_node != NULL; this_node = this_node->next)
  {
    callback(position, this_node->tok_elt, cb_data);
    position++;
  }
}