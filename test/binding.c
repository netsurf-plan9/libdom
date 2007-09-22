#include <stdio.h>

#include <dom/dom.h>
#include "testutils.h"

int main(int argc, char **argv)
{
	struct dom_document *doc;
	struct dom_element *element;
	struct dom_string *elementName;
	dom_exception err;
	TestObject *staff;

	staff = test_object_create(argc, argv, "staff.xml", false);
	assert(staff != NULL);

	doc = test_object_get_doc(staff);
	assert(doc != NULL);

	err = dom_document_get_document_element(doc, &element);
	assert(err == DOM_NO_ERR);
	assert(element != NULL);

	err = dom_element_get_tag_name(element, &elementName);
	assert(err == DOM_NO_ERR);
	assert(elementName != NULL);

	printf("PASS\n");

	return 0;
}
