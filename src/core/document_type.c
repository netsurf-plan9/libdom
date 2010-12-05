/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 * Copyright 2007 James Shaw <jshaw@netsurf-browser.org>
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#include <assert.h>

#include <dom/core/document_type.h>

#include "core/string.h"
#include "core/document_type.h"
#include "core/node.h"
#include "utils/utils.h"
#include "utils/namespace.h"
#include "utils/resource_mgr.h"

/**
 * DOM DocumentType node
 */
struct dom_document_type {
	struct dom_node_internal base;		/**< Base node */

	struct dom_string *public_id;	/**< Doctype public ID */
	struct dom_string *system_id;	/**< Doctype system ID */

	struct dom_resource_mgr res;	/**< resource_mgr of this node */
};

static struct dom_document_type_vtable document_type_vtable = {
	{
		DOM_NODE_VTABLE	
	},
	DOM_DOCUMENT_TYPE_VTABLE
};

static struct dom_node_protect_vtable dt_protect_vtable = {
	DOM_DT_PROTECT_VTABLE
};


/*----------------------------------------------------------------------*/

/* Constructors and destructors */

/**
 * Create a document type node
 *
 * \param qname      The qualified name of the document type
 * \param public_id  The external subset public identifier
 * \param system_id  The external subset system identifier
 * \param alloc      Memory (de)allocation function
 * \param pw         Pointer to client-specific private data
 * \param doctype    Pointer to location to receive result
 * \return DOM_NO_ERR on success, DOM_NO_MEM_ERR on memory exhaustion.
 *
 * The doctype will be referenced, so the client need not do so
 * explicitly. The client must unref the doctype once it has
 * finished with it.
 */
dom_exception _dom_document_type_create(struct dom_string *qname,
		struct dom_string *public_id, struct dom_string *system_id,
		dom_alloc alloc, void *pw,
		struct dom_document_type **doctype)
{
	struct dom_document_type *result;
	dom_exception err;

	/* Create node */
	result = alloc(NULL, sizeof(struct dom_document_type), pw);
	if (result == NULL)
		return DOM_NO_MEM_ERR;

	/* Initialise the vtable */
	result->base.base.vtable = &document_type_vtable;
	result->base.vtable = &dt_protect_vtable;
	
	err = _dom_document_type_initialise(result, qname, public_id, system_id,
			alloc, pw);

	*doctype = result;

	return DOM_NO_ERR;
}

/**
 * Destroy a DocumentType node
 *
 * \param doctype  The DocumentType node to destroy
 *
 * The contents of ::doctype will be destroyed and ::doctype will be freed.
 */
void _dom_document_type_destroy(struct dom_node_internal *doctypenode)
{
	struct dom_document_type *doctype = 
			(struct dom_document_type *)doctypenode;

	/* Finalise base class */
	_dom_document_type_finalise(doctype);

	/* Free doctype */
	doctype->res.alloc(doctype, 0, doctype->res.pw);
}

/* Initialise this document_type */
dom_exception _dom_document_type_initialise(struct dom_document_type *doctype,
		struct dom_string *qname, struct dom_string *public_id,
		struct dom_string *system_id, dom_alloc alloc, void *pw)
{
	dom_exception err;

	dom_string *prefix, *localname;
	err = _dom_namespace_split_qname(qname, &prefix, &localname);
	if (err != DOM_NO_ERR) {
		alloc(doctype, 0, pw);
		return err;
	}

	lwc_string *lprefix = NULL, *lname = NULL;
	if (prefix != NULL) {
		err = _dom_string_intern(prefix, &lprefix);
		if (err != DOM_NO_ERR) {
			dom_string_unref(prefix);
			dom_string_unref(localname);
			alloc(doctype, 0, pw);
			return err;
		}
	}

	if (localname != NULL) {
		err = _dom_string_intern(localname, &lname);
		if (err != DOM_NO_ERR) {
			dom_string_unref(prefix);
			dom_string_unref(localname);
			if (lprefix != NULL)
				lwc_string_unref(lprefix);
			alloc(doctype, 0, pw);
			return err;
		}
	}

	/* TODO: I should figure out how the namespaceURI can be got */

	/* Initialise base node */
	err = _dom_node_initialise_generic(&doctype->base, NULL, alloc, pw,
			DOM_DOCUMENT_TYPE_NODE, lname, NULL, NULL, lprefix);
	if (err != DOM_NO_ERR) {
		alloc(doctype, 0, pw);
		return err;
	}

	/* Get public and system IDs */
	if (public_id != NULL)
		dom_string_ref(public_id);
	doctype->public_id = public_id;

	if (system_id != NULL)
		dom_string_ref(system_id);
	doctype->system_id = system_id;

	if (prefix != NULL)
		dom_string_unref(prefix);
	if (localname != NULL)
		dom_string_unref(localname);

	/* Fill in allocation information */
	doctype->res.alloc = alloc;
	doctype->res.pw = pw;

	return DOM_NO_ERR;
}

/* The destructor function of dom_document_type */
void _dom_document_type_finalise(struct dom_document_type *doctype)
{
	if (doctype->public_id != NULL)
		dom_string_unref(doctype->public_id);
	if (doctype->system_id != NULL)
		dom_string_unref(doctype->system_id);
	
	assert(doctype->base.owner != NULL || doctype->base.user_data == NULL);
	
	_dom_node_finalise_generic(&doctype->base, doctype->res.alloc, 
			doctype->res.pw);
}


/*----------------------------------------------------------------------*/

/* Virtual functions */

/**
 * Retrieve a document type's name
 *
 * \param doc_type  Document type to retrieve name from
 * \param result    Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 *
 * We don't support this API now, so this function call should always
 * return DOM_NOT_SUPPORTED_ERR.
 */
dom_exception _dom_document_type_get_name(struct dom_document_type *doc_type,
		struct dom_string **result)
{
	return dom_node_get_node_name(doc_type, result);
}

/**
 * Retrieve a document type's entities
 *
 * \param doc_type  Document type to retrieve entities from
 * \param result    Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned map will have its reference count increased. It is
 * the responsibility of the caller to unref the map once it has
 * finished with it.
 *
 * We don't support this API now, so this function call should always
 * return DOM_NOT_SUPPORTED_ERR.
 */
dom_exception _dom_document_type_get_entities(
		struct dom_document_type *doc_type,
		struct dom_namednodemap **result)
{
	UNUSED(doc_type);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Retrieve a document type's notations
 *
 * \param doc_type  Document type to retrieve notations from
 * \param result    Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned map will have its reference count increased. It is
 * the responsibility of the caller to unref the map once it has
 * finished with it.
 *
 * We don't support this API now, so this function call should always
 * return DOM_NOT_SUPPORTED_ERR.
 */
dom_exception _dom_document_type_get_notations(
		struct dom_document_type *doc_type,
		struct dom_namednodemap **result)
{
	UNUSED(doc_type);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Retrieve a document type's public id
 *
 * \param doc_type  Document type to retrieve public id from
 * \param result    Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 *
 * We don't support this API now, so this function call should always
 * return DOM_NOT_SUPPORTED_ERR.
 */
dom_exception _dom_document_type_get_public_id(
		struct dom_document_type *doc_type,
		struct dom_string **result)
{
	UNUSED(doc_type);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Retrieve a document type's system id
 *
 * \param doc_type  Document type to retrieve system id from
 * \param result    Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 *
 * We don't support this API now, so this function call should always
 * return DOM_NOT_SUPPORTED_ERR.
 */
dom_exception _dom_document_type_get_system_id(
		struct dom_document_type *doc_type,
		struct dom_string **result)
{
	UNUSED(doc_type);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Retrieve a document type's internal subset
 *
 * \param doc_type  Document type to retrieve internal subset from
 * \param result    Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 *
 * We don't support this API now, so this function call should always
 * return DOM_NOT_SUPPORTED_ERR.
 */
dom_exception _dom_document_type_get_internal_subset(
		struct dom_document_type *doc_type,
		struct dom_string **result)
{
	UNUSED(doc_type);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/*-----------------------------------------------------------------------*/

/* Overload protected virtual functions */

/* The virtual destroy function of this class */
void _dom_dt_destroy(struct dom_node_internal *node)
{
	_dom_document_type_destroy(node);
}

/* The memory allocator of this class */
dom_exception _dom_dt_alloc(struct dom_document *doc,
		struct dom_node_internal *n, struct dom_node_internal **ret)
{
	UNUSED(doc);
	UNUSED(n);
	UNUSED(ret);

	return DOM_NOT_SUPPORTED_ERR;
}

/* The copy constructor of this class */
dom_exception _dom_dt_copy(struct dom_node_internal *new, 
		struct dom_node_internal *old)
{
	UNUSED(new);
	UNUSED(old);

	return DOM_NOT_SUPPORTED_ERR;
}


/*----------------------------------------------------------------------*/

/* Helper functions */

/* Get the resource manager of this object */
void _dom_document_type_get_resource_mgr(
		struct dom_document_type *dt, struct dom_resource_mgr *rm)
{
	rm->alloc = dt->res.alloc;
	rm->pw = dt->res.pw;
}

