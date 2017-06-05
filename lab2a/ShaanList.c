/* NAME: SHAAN MATHUR
   EMAIL: SHAANKARANMATHUR@GMAIL.COM
   UID: 904606576
*/


#include "SortedList.h"
#include "string.h"
#include <sched.h>

/**
 * SortedList_insert ... insert an element into a sorted list
 *
 *The specified element will be inserted in to
 *the specified list, which will be kept sorted
 *in ascending order based on associated keys
 *
 * @param SortedList_t *list ... header for the list
 * @param SortedListElement_t *element ... element to be added to the list
 */
void SortedList_insert(SortedList_t *list, SortedListElement_t *element){
 
  
  while(list->next != NULL && strcmp(list->next->key, element->key) < 0)
    list = list->next;


  if(opt_yield & INSERT_YIELD)
    sched_yield();
  // list->next == NULL || list->next->key <= element->key
  element->next = list->next;
  element->prev = list;
  list->next = element;
  
  if(element->next != NULL){
    element->next->prev = element;
  }
}

/**
 * SortedList_delete ... remove an element from a sorted list
 *
 *The specified element will be removed from whatever
 *list it is currently in.
 *
 *Before doing the deletion, we check to make sure that
 *next->prev and prev->next both point to this node
 *
 * @param SortedListElement_t *element ... element to be removed
 *
 * @return 0: element deleted successfully, 1: corrtuped prev/next pointers
 *
 */
int SortedList_delete( SortedListElement_t *element){
  //printf("Entering delete\n");
  if((element->next != NULL && element->next->prev != element) || (element->prev != NULL && element->prev->next != element))
    return 1;

  if(opt_yield & DELETE_YIELD)
    sched_yield();
  
  if(element->next != NULL){
    element->next->prev = element->prev;
  }
  if(element->prev != NULL){
    element->prev->next = element->next;
  }

  return 0;
}

/**
 * SortedList_lookup ... search sorted list for a key
 *
 *The specified list will be searched for an
 *element with the specified key.
 *
 * @param SortedList_t *list ... header for the list
 * @param const char * key ... the desired key
 *
 * @return pointer to matching element, or NULL if none is found
 */
SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key){
  int res;
  
  while(list->next != NULL && (res = strcmp(list->next->key, key)) < 0){
    if(opt_yield & LOOKUP_YIELD)
      sched_yield();
    list = list->next;
  }

  // list->next == NULL || res <= 0
  if(res > 0)
    return NULL;
  else
    return list->next;
}

/**
 * SortedList_length ... count elements in a sorted list
 *While enumeratign list, it checks all prev/next pointers
 *
 * @param SortedList_t *list ... header for the list
 *
 * @return int number of elements in list (excluding head)
 *   -1 if the list is corrupted
 */
int SortedList_length(SortedList_t *list){
  int count = 0;
  
  while(list->next != NULL){
    count++;
    if(opt_yield & LOOKUP_YIELD)
      sched_yield();
  
    if(list->next->prev != list)
      return -1;
    list = list->next;
  }

  return count;
}
