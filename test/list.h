/*
 * This file is part of libdom test suite.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 James Shaw <jshaw@netsurf-browser.org>
 */

#ifndef list_h_
#define list_h_

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

typedef int (*list_compare_func)(const void* a, const void* b);

/**
 * Add data to the tail of the list.
 */
void list_add(struct list* list, void* data);

/**
 * Tests if data is equal to any element in the list.
 */
bool list_contains(struct list* list, void* data, 
		int (*comparator)(const void* a, const void* b));

/**
 * Tests if superlist contains all elements in sublist.  Order is not important.
 */
bool list_contains_all(struct list* superList, struct list* subList, 
		list_compare_func comparator);

#endif
