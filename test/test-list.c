#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "testutils.h"

void test_add_remove(void);
void test_contains_all_true(void);

void test_add_remove(void)
{
	struct list* list = list_new();
	
	char s[] = "hello"; 
	
	/* add element */
	list_add(list, s);
	assert(strcmp(list->head->data, "hello") == 0);
	assert(list->size == 1);
	
	/* remove element */
	bool found = list_remove(list, s);
	assert(found == true);
	assert(list->size == 0);
	assert(list->head == NULL);
	
	list_destroy(list);
}

void test_contains_all_true(void)
{
	struct list* superList = list_new();
	struct list* subList = list_new();
	
	list_add(superList, (void*) "hello");
	list_add(superList, (void*) "world");
	
	list_add(subList,   (void*) "hello");
	
	bool b = list_contains_all(superList, subList, (comparator) strcmp);
	assert(b == true);
	assert(superList->size == 2);
	assert(superList->head->next->next == NULL);
		
	list_destroy(superList);
	list_destroy(subList);
}

int main(void)
{
	test_add_remove();
	test_contains_all_true();
	//test_different_size_lists();
	
	printf("PASS\n");
}
