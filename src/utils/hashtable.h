/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2006 Rob Kendrick <rjek@rjek.com>
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#ifndef dom_utils_hashtable_h_
#define dom_utils_hashtable_h_

#include <stdbool.h>
#include <dom/functypes.h>

typedef struct dom_hash_table dom_hash_table;
/* The hash function */
typedef unsigned int (*dom_hash_func)(void *key);
/* Function to clone/delete key */
typedef void *(*dom_key_func)(void *key, void *pw, dom_alloc alloc, 
		void *alloc_pw, bool clone);
/* Function to clone/delete value */
typedef void *(*dom_value_func)(void *value, void *pw, dom_alloc alloc,
		void *alloc_pw, bool clone);

struct dom_hash_table *_dom_hash_create(unsigned int chains, dom_hash_func hash,
		dom_alloc alloc, void *ptr);
struct dom_hash_table *_dom_hash_clone(struct dom_hash_table *ht, 
		dom_alloc alloc, void *pw, dom_key_func kf, void *key_pw, 
		dom_value_func vf, void *value_pw);
void _dom_hash_destroy(struct dom_hash_table *ht, dom_key_func kf, void *key_pw,
		dom_value_func vf, void *value_pw);
bool _dom_hash_add(struct dom_hash_table *ht, void *key, void *value, 
		bool replace);
void *_dom_hash_get(struct dom_hash_table *ht, void *key);
void *_dom_hash_del(struct dom_hash_table *ht, void *key);
void *_dom_hash_iterate(struct dom_hash_table *ht, unsigned int *c1,
		unsigned int **c2);
unsigned int _dom_hash_get_length(struct dom_hash_table *ht);
unsigned int _dom_hash_get_chains(struct dom_hash_table *ht);
dom_hash_func _dom_hash_get_func(struct dom_hash_table *ht);

#endif
