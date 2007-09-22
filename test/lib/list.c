/*
 * This file is part of libdom test suite.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 James Shaw <jshaw@netsurf-browser.org>
 */


#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "list.h"
#include "testassert.h"

struct list* list_new(void)
{
	struct list* list = malloc(sizeof(struct list));
	assert(list != NULL);
	list->size = 0;
	list->head = NULL;
	list->tail = NULL;
	return list;
}

void list_destroy(struct list* list)
{
	struct list_elt* elt = list->head;
	while (elt != NULL) {
		struct list_elt* nextElt = elt->next;
		free(elt);
		elt = nextElt;
	}
	free(list);
}

void list_add(struct list* list, void* data)
{
	struct list_elt* elt = malloc(sizeof(struct list_elt));
	assert(elt != NULL);
	elt->data = data;
	elt->next = NULL;
	struct list_elt* tail = list->tail;

	/* if tail was set, make its 'next' ptr point to elt */
	if (tail != NULL) {
		tail->next = elt;
	}

	/* make elt the new tail */
	list->tail = elt;

	if (list->head == NULL) {
		list->head = elt;
	}

	/* inc the size of the list */
	list->size++;
}

bool list_contains(struct list* list, void* data, list_compare_func comparator)
{
	struct list_elt* elt = list->head;
	while (elt != NULL) {
		if (comparator(elt->data, data) == 0) {
			return true;
		}
		elt = elt->next;
	}
	return false;
}

bool list_contains_all(struct list* superList, struct list* subList, 
		list_compare_func comparator)
{
	struct list_elt* elt = subList->head;
	while (elt != NULL) {
		if (!list_contains(superList, elt->data, comparator)) {
			return false;
		}
		elt = elt->next;
	}
	return true;
}

