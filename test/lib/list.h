/*
 * This file is part of libdom test suite.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 James Shaw <jshaw@netsurf-browser.org>
 */

#ifndef list_h_
#define list_h_

#include <stdbool.h>

#include "comparators.h"

struct list_elt {
	void* data;
	struct list_elt* next;
};

struct list {
	unsigned int size;
	struct list_elt* head;
	struct list_elt* tail;
};

struct list* list_new(void);
void list_destroy(struct list* list);

/**
 * Add data to the tail of the list.
 */
void list_add(struct list* list, void* data);

/**
 * Remove element containing data from list.
 * The list element is freed, but the caller must free the data itself
 * if necessary.
 * 
 * Returns true if data was found in the list.
 */
bool list_remove(struct list* list, void* data);

struct list* list_clone(struct list* list);

/**
 * Tests if data is equal to any element in the list.
 */
bool list_contains(struct list* list, void* data,
		comparator comparator);

/**
 * Tests if superlist contains all elements in sublist.  Order is not important.
 */
bool list_contains_all(struct list* superList, struct list* subList, 
		comparator comparator);

#endif
