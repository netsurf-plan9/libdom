/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#include <assert.h>

#include <dom/core/characterdata.h>
#include <dom/core/string.h>

#include "core/characterdata.h"
#include "core/document.h"
#include "core/node.h"
#include "utils/utils.h"

/* The virtual functions for dom_characterdata, we make this vtable
 * public to each child class */
struct dom_characterdata_vtable characterdata_vtable = {
	{
		DOM_NODE_VTABLE
	},
	DOM_CHARACTERDATA_VTABLE
};


/* Create a DOM characterdata node and compose the vtable */
dom_characterdata *_dom_characterdata_create(struct dom_document *doc)
{
	dom_characterdata *cdata = _dom_document_alloc(doc, NULL,
			sizeof(struct dom_characterdata));

	if (cdata == NULL)
		return NULL;

	cdata->base.base.vtable = &characterdata_vtable;
	cdata->base.vtable = NULL;

	return cdata;
}

/**
 * Initialise a character data node
 *
 * \param node   The node to initialise
 * \param doc    The document which owns the node
 * \param type   The node type required
 * \param name   The node name, or NULL
 * \param value  The node value, or NULL
 * \return DOM_NO_ERR on success.
 *
 * ::doc, ::name and ::value will have their reference counts increased.
 */
dom_exception _dom_characterdata_initialise(struct dom_characterdata *cdata,
		struct dom_document *doc, dom_node_type type,
		lwc_string *name, struct dom_string *value)
{
	return _dom_node_initialise(&cdata->base, doc, type, 
			name, value, NULL, NULL);
}

/**
 * Finalise a character data node
 *
 * \param doc    The owning document
 * \param cdata  The node to finalise
 *
 * The contents of ::cdata will be cleaned up. ::cdata will not be freed.
 */
void _dom_characterdata_finalise(struct dom_document *doc,
		struct dom_characterdata *cdata)
{
	_dom_node_finalise(doc, &cdata->base);
}


/*----------------------------------------------------------------------*/

/* The public virtual functions */

/**
 * Retrieve data from a character data node
 *
 * \param cdata  Character data node to retrieve data from
 * \param data   Pointer to location to receive data
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 *
 * DOM3Core states that this can raise DOMSTRING_SIZE_ERR. It will not in
 * this implementation; dom_strings are unbounded.
 */
dom_exception _dom_characterdata_get_data(struct dom_characterdata *cdata,
		struct dom_string **data)
{
	struct dom_node_internal *c = (struct dom_node_internal *) cdata;

	if (c->value != NULL) {
		dom_string_ref(c->value);
	}
	*data = c->value;

	return DOM_NO_ERR;
}

/**
 * Set the content of a character data node
 *
 * \param cdata  Node to set the content of
 * \param data   New value for node
 * \return DOM_NO_ERR                      on success,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::cdata is readonly.
 *
 * The new content will have its reference count increased, so the caller
 * should unref it after the call (as the caller should have already claimed
 * a reference on the string). The node's existing content will be unrefed.
 */
dom_exception _dom_characterdata_set_data(struct dom_characterdata *cdata,
		struct dom_string *data)
{
	struct dom_node_internal *c = (struct dom_node_internal *) cdata;

	if (_dom_node_readonly(c)) {
		return DOM_NO_MODIFICATION_ALLOWED_ERR;
	}

	if (c->value != NULL) {
		dom_string_unref(c->value);
	}

	dom_string_ref(data);
	c->value = data;

	return DOM_NO_ERR;
}

/**
 * Get the length (in characters) of a character data node's content
 *
 * \param cdata   Node to read content length of
 * \param length  Pointer to location to receive character length of content
 * \return DOM_NO_ERR.
 */
dom_exception _dom_characterdata_get_length(struct dom_characterdata *cdata,
		unsigned long *length)
{
	struct dom_node_internal *c = (struct dom_node_internal *) cdata;

	if (c->value != NULL) {
		*length = dom_string_length(c->value);
	} else {
		*length = 0;
	}

	return DOM_NO_ERR;
}

/**
 * Extract a range of data from a character data node
 *
 * \param cdata   The node to extract data from
 * \param offset  The character offset of substring to extract
 * \param count   The number of characters to extract
 * \param data    Pointer to location to receive substring
 * \return DOM_NO_ERR         on success,
 *         DOM_INDEX_SIZE_ERR if ::offset is greater than the number of
 *                            characters in ::cdata.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 *
 * DOM3Core states that this can raise DOMSTRING_SIZE_ERR. It will not in
 * this implementation; dom_strings are unbounded.
 */
dom_exception _dom_characterdata_substring_data(
		struct dom_characterdata *cdata, unsigned long offset,
		unsigned long count, struct dom_string **data)
{
	struct dom_node_internal *c = (struct dom_node_internal *) cdata;
	uint32_t len, end;

	if (c->value != NULL) {
		len = dom_string_length(c->value);
	} else {
		len = 0;
	}

	if (offset > len) {
		return DOM_INDEX_SIZE_ERR;
	}

	end = (offset + count) >= len ? len : offset + count;

	return dom_string_substr(c->value, offset, end, data);
}

/**
 * Append data to the end of a character data node's content
 *
 * \param cdata  The node to append data to
 * \param data   The data to append
 * \return DOM_NO_ERR                      on success,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::cdata is readonly.
 */
dom_exception _dom_characterdata_append_data(struct dom_characterdata *cdata,
		struct dom_string *data)
{
	struct dom_node_internal *c = (struct dom_node_internal *) cdata;
	struct dom_string *temp;
	dom_exception err;

	if (_dom_node_readonly(c)) {
		return DOM_NO_MODIFICATION_ALLOWED_ERR;
	}

	err = dom_string_concat(c->value, data, &temp);
	if (err != DOM_NO_ERR) {
		return err;
	}

	if (c->value != NULL) {
		dom_string_unref(c->value);
	}

	c->value = temp;

	return DOM_NO_ERR;
}

/**
 * Insert data into a character data node's content
 *
 * \param cdata   The node to insert into
 * \param offset  The character offset to insert at
 * \param data    The data to insert
 * \return DOM_NO_ERR                      on success,
 *         DOM_INDEX_SIZE_ERR              if ::offset is greater than the
 *                                         number of characters in ::cdata,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::cdata is readonly.
 */
dom_exception _dom_characterdata_insert_data(struct dom_characterdata *cdata,
		unsigned long offset, struct dom_string *data)
{
	struct dom_node_internal *c = (struct dom_node_internal *) cdata;
	struct dom_string *temp;
	uint32_t len;
	dom_exception err;

	if (_dom_node_readonly(c)) {
		return DOM_NO_MODIFICATION_ALLOWED_ERR;
	}

	if (c->value != NULL) {
		len = dom_string_length(c->value);
	} else {
		len = 0;
	}

	if (offset > len) {
		return DOM_INDEX_SIZE_ERR;
	}

	err = dom_string_insert(c->value, data, offset, &temp);
	if (err != DOM_NO_ERR) {
		return err;
	}

	if (c->value != NULL) {
		dom_string_unref(c->value);
	}

	c->value = temp;

	return DOM_NO_ERR;
}

/**
 * Delete data from a character data node's content
 *
 * \param cdata   The node to delete from
 * \param offset  The character offset to start deletion from
 * \param count   The number of characters to delete
 * \return DOM_NO_ERR                      on success,
 *         DOM_INDEX_SIZE_ERR              if ::offset is greater than the
 *                                         number of characters in ::cdata,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::cdata is readonly.
 */
dom_exception _dom_characterdata_delete_data(struct dom_characterdata *cdata,
		unsigned long offset, unsigned long count)
{
	struct dom_node_internal *c = (struct dom_node_internal *) cdata;
	struct dom_string *temp;
	uint32_t len, end;
	dom_exception err;

	if (_dom_node_readonly(c)) {
		return DOM_NO_MODIFICATION_ALLOWED_ERR;
	}

	if (c->value != NULL) {
		len = dom_string_length(c->value);
	} else {
		len = 0;
	}

	if (offset > len) {
		return DOM_INDEX_SIZE_ERR;
	}

	end = (offset + count) >= len ? len : offset + count;

	err = dom_string_replace(c->value, NULL, offset, end, &temp);
	if (err != DOM_NO_ERR) {
		return err;
	}

	if (c->value != NULL) {
		dom_string_unref(c->value);
	}

	c->value = temp;

	return DOM_NO_ERR;
}

/**
 * Replace a section of a character data node's content
 *
 * \param cdata   The node to modify
 * \param offset  The character offset of the sequence to replace
 * \param count   The number of characters to replace
 * \param data    The replacement data
 * \return DOM_NO_ERR                      on success,
 *         DOM_INDEX_SIZE_ERR              if ::offset is greater than the
 *                                         number of characters in ::cdata,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::cdata is readonly.
 */
dom_exception _dom_characterdata_replace_data(struct dom_characterdata *cdata,
		unsigned long offset, unsigned long count,
		struct dom_string *data)
{
	struct dom_node_internal *c = (struct dom_node_internal *) cdata;
	struct dom_string *temp;
	uint32_t len, end;
	dom_exception err;

	if (_dom_node_readonly(c)) {
		return DOM_NO_MODIFICATION_ALLOWED_ERR;
	}

	if (c->value != NULL) {
		len = dom_string_length(c->value);
	} else {
		len = 0;
	}

	if (offset > len) {
		return DOM_INDEX_SIZE_ERR;
	}

	end = (offset + count) >= len ? len : offset + count;

	err = dom_string_replace(c->value, data, offset, end, &temp);
	if (err != DOM_NO_ERR) {
		return err;
	}

	if (c->value != NULL) {
		dom_string_unref(c->value);
	}

	c->value = temp;

	return DOM_NO_ERR;
}



/*----------------------------------------------------------------------*/

/* The protected virtual functions of Node, see core/node.h for details 
 *
 * @note: the three following API never be called directly from the virtual
 *	  functions dispatch mechanism, they are here for the code consistent.
 */
void _dom_characterdata_destroy(struct dom_node_internal *node)
{
	assert("Should never be here" == NULL);
	UNUSED(node);
}

/* The memory allocator of this class */
dom_exception _dom_characterdata_alloc(struct dom_document *doc,
		struct dom_node_internal *n, struct dom_node_internal **ret)
{
	assert("Should never be here" == NULL);
	UNUSED(doc);
	UNUSED(n);
	UNUSED(ret);

	return DOM_NO_ERR;
}

/* The copy constructor of this class
 * The sub-class of characterdata should call this API */
dom_exception _dom_characterdata_copy(struct dom_node_internal *new, 
		struct dom_node_internal *old)
{
	return _dom_node_copy(new, old);
}

