/*
        Copyright (c) 1996-1997 Swiss Federal Institute of Technology, 
        Computer Engineering and Networks Laboratory. All rights reserved.

        TOPSY - A Teachable Operating System. 
                Implementation of a tiny and simple micro kernel for
                teaching purposes.

        Permission to use, copy, modify, and distribute this software and its
        documentation for any purpose, without fee, and without written 
        agreement is hereby granted, provided that the above copyright notice 
        and the following two paragraphs appear in all copies of this software.


        
        File:                  $Source: /proj/topsy/ss98/Topsy/Topsy/RCS/List.c,v $
        Author(s):             George Fankhauser
        Affiliation:           ETH Zuerich, TIK
        Version:               $Revision: 1.7 $
        Creation Date:         
        Last Date of Change:   $Date: 1997/03/31 20:27:18 $      by: $Author: gfa $
        
        
*/

#include "List.h"

/* forward declarations */
static void removeElem(List list, ListElement hintElem);

List listNew()
{
    List list;
    Error error = hmAlloc((Address*)&list, sizeof(ListDesc));
    
    if (error != HM_ALLOCOK) return NULL;
    list->first = NULL;
    list->current = NULL;
    list->last = NULL;

    return list;
}


Error listFree(List list)
{
    ListElement elem, prev;

    if (list == NULL) return LIST_ERROR;
    elem = list->first;
    while (elem != NULL) {
    	prev = elem;
	elem = elem->next;
	hmFree(prev);
    }
    hmFree(list);
    return LIST_OK;
}


Error listAddInFront(List list, void* item, Address* hint)
{
    ListElement elem;
    Error error;
    
    if (list == NULL) return LIST_ERROR;
    error = hmAlloc((Address*)&elem, sizeof(ListDesc));
    if (error != HM_ALLOCOK) return LIST_ERROR;
    
    /* if the user of the list provides space to hint the position
     * of an element later, we write to the specified address
     */
    if (hint != NULL) *hint = elem;
    
    elem->next = list->first;
    elem->prev = NULL;
    
    if (list->first != NULL) {
	list->first->prev = elem;
    }
    else {
	list->last = elem;
    }
    list->first = elem;
    
    elem->item = item;
    
    return LIST_OK;
}


Error listAddAtEnd(List list, void* item, Address* hint)
{
    ListElement elem;
    Error error;
    
    if (list == NULL) return LIST_ERROR;
    if (list->first == NULL) {
	return (listAddInFront(list, item, hint));
    }
    else {
	error = hmAlloc((Address*)&elem, sizeof(ListDesc));
	if (error != HM_ALLOCOK) return LIST_ERROR;
	
	/* if the user of the list provides space to hint the position
	 * of an element later, we write to the specified address
	 */
	if (hint != NULL) *hint = elem;

	elem->next = NULL;
	elem->prev = list->last;
	list->last->next = elem;
	list->last = elem;
	
	elem->item = item;
    }
    return LIST_OK;
}


Error listRemove(List list, void* item, Address hint)
{
    ListElement elem, prev, hintElem;
    
    if (list == NULL || item == NULL) return LIST_ERROR;
    hintElem = (ListElement)hint;
    
    /* hint is valid, use direct access to doubly linked list */
    if (hint != NULL && (hintElem->item == item)) {
    	/* remove element from listFrom */
	removeElem(list, hintElem);
	hintElem->item = NULL;
	hmFree(hintElem);
	return LIST_OK;
    }
    /* we have to search the list linearly */
    else {
	if (list->first == NULL) return LIST_ERROR;
	if (list->first->item == item) {
	    prev = list->first;
	    list->first = prev->next;
	    if (list->first != NULL) list->first->prev = NULL; /***/
	    else list->last = NULL; /***/
	    prev->item = NULL;
	    hmFree(prev);
	    return LIST_OK;
	} 
	prev = list->first;
	elem = prev->next;
	while (elem != NULL) {
	    if (elem->item == item) {
		prev->next = elem->next;
		if (prev->next != NULL) prev->next->prev = prev; /***/
		else list->last = prev; /***/
		elem->item = NULL;
		hmFree(elem);
		return LIST_OK;
	    }
	    prev = prev->next;
	    elem = elem->next;
	}
    } 
    return LIST_ERROR;
}


Error listMoveToEnd(List list, void* item, Address hint)
{
    ListElement hintElem;
    
    if (list == NULL || item == NULL) return LIST_ERROR;
    hintElem = (ListElement)hint;
        
    /* hint is valid, use direct access to doubly linked list */
    if (hint != NULL && (hintElem->item == item)) {
	if (list->last == hintElem) return LIST_OK;       /* already at end */
	
	/* remove element */
	if (list->first == hintElem) {
	    list->first->next->prev = NULL;
	    list->first = list->first->next;
	}
	else {
	    hintElem->next->prev = hintElem->prev;
	    hintElem->prev->next = hintElem->next;
	}
	/* add at end */
	list->last->next = hintElem;
	hintElem->prev = list->last;
	hintElem->next = NULL;
	list->last = hintElem;
	return LIST_OK;
    }
    /* linear search not supported */
    return LIST_ERROR;
}


Error listSwap(List listFrom, List listTo, void* item, Address hint)
{
    ListElement hintElem;

    if (listFrom == NULL || listTo == NULL || item == NULL) return LIST_ERROR;
    hintElem = (ListElement)hint;
        
    /* hint is valid, use direct access to doubly linked list */
    if (hint != NULL && (hintElem->item == item)) {
    	/* remove element from listFrom */
	removeElem(listFrom, hintElem);
	/* add it to listTo (in front) */	
	hintElem->next = listTo->first;
	if (listTo->first != NULL) listTo->first->prev = hintElem;
	listTo->first = hintElem;
	hintElem->prev = NULL;
	/* if the new one is the only element, set last to first */
	if (listTo->last == NULL) listTo->last = hintElem;
	
	return LIST_OK;
    }
    /* linear search not supported */
    return LIST_ERROR;
}


static void removeElem(List list, ListElement hintElem)
{
    /* first element */
    if (hintElem == list->first) {
	list->first = hintElem->next;
	if (hintElem->next != NULL) hintElem->next->prev = NULL;
	else list->last = NULL;
    }
    /* last element */
    else if (hintElem == list->last) {
	list->last = hintElem->prev;
	if (hintElem->prev != NULL) hintElem->prev->next = NULL;
	else list->first = NULL;	    
    }
    /* all the others in between */
    else {
	hintElem->next->prev = hintElem->prev;
	hintElem->prev->next = hintElem->next;
    }
}


Error listGetFirst(List list, void** itemPtr)
{
    if (list == NULL) return LIST_ERROR;
    if (list->first == NULL) {
	*itemPtr = NULL;
    }
    else {
	*itemPtr = list->first->item;
    }
    list->current = list->first;
    return LIST_OK;
}


Error listGetNext(List list, void** itemPtr)
{
    if (list == NULL) return LIST_ERROR;
    if (list->current != NULL) list->current = list->current->next;
    if (list->current == NULL) {
	*itemPtr = NULL;
    }
    else {
	*itemPtr = list->current->item;
    }
    return LIST_OK;
}
