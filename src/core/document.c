/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#include <string.h>

#include <dom/functypes.h>
#include <dom/bootstrap/implpriv.h>
#include <dom/core/document.h>
#include <dom/core/implementation.h>
#include <dom/core/string.h>

#include "core/attr.h"
#include "core/cdatasection.h"
#include "core/comment.h"
#include "core/document.h"
#include "core/doc_fragment.h"
#include "core/element.h"
#include "core/entity_ref.h"
#include "core/namednodemap.h"
#include "core/node.h"
#include "core/nodelist.h"
#include "core/pi.h"
#include "core/text.h"
#include "utils/namespace.h"
#include "utils/utils.h"

struct dom_document_type;

/**
 * Item in list of active nodelists
 */
struct dom_doc_nl {
	struct dom_nodelist *list;	/**< Nodelist */

	struct dom_doc_nl *next;	/**< Next item */
	struct dom_doc_nl *prev;	/**< Previous item */
};

/**
 * Iten in list of active namednodemaps
 */
struct dom_doc_nnm {
	struct dom_namednodemap *map;	/**< Named node map */

	struct dom_doc_nnm *next;	/**< Next map */
	struct dom_doc_nnm *prev;	/**< Previous map */
};

/**
 * DOM document
 */
struct dom_document {
	struct dom_node base;		/**< Base node */

	dom_string_charset charset;	/**< Charset of strings in document */

	struct dom_implementation *impl;	/**< Owning implementation */

	struct dom_doc_nl *nodelists;	/**< List of active nodelists */

	struct dom_doc_nnm *maps;	/**< List of active namednodemaps */

	struct dom_string **nodenames;	/**< Interned nodenames */

	dom_alloc alloc;		/**< Memory (de)allocation function */
	void *pw;			/**< Pointer to client data */
};

/** Interned node name strings, indexed by node type */
/* Index 0 is unused */
static struct dom_string *__nodenames_utf8[DOM_NODE_TYPE_COUNT + 1];
static struct dom_string *__nodenames_utf16[DOM_NODE_TYPE_COUNT + 1];

/**
 * Initialise the document module
 *
 * \param alloc  Memory (de)allocation function
 * \param pw     Pointer to client-specific private data
 * \return DOM_NO_ERR on success
 */
dom_exception _dom_document_initialise(dom_alloc alloc, void *pw)
{
	static struct {
		const char *name;
		size_t len;
	} names_utf8[DOM_NODE_TYPE_COUNT + 1] = {
		{ NULL,			0 },	/* Unused */
		{ NULL,			0 },	/* Element */
		{ NULL,			0 },	/* Attr */
		{ "#text",		5 },	/* Text */
		{ "#cdata-section",	14 },	/* CDATA section */
		{ NULL,			0 },	/* Entity reference */
		{ NULL,			0 },	/* Entity */
		{ NULL,			0 },	/* Processing instruction */
		{ "#comment",		8 },	/* Comment */
		{ "#document",		9 },	/* Document */
		{ NULL,			0 },	/* Document type */
		{ "#document-fragment",	18 },	/* Document fragment */
		{ NULL,			0 }	/* Notation */
	};

	/** \todo This assumes Little Endian */
	static struct {
		const char *name;
		size_t len;
	} names_utf16[DOM_NODE_TYPE_COUNT + 1] = {
		{ NULL,			0 },	/* Unused */
		{ NULL,			0 },	/* Element */
		{ NULL,			0 },	/* Attr */
		{ "#\0t\0e\0x\0t\0",	10 },	/* Text */
		{ "#\0c\0d\0a\0t\0a\0-\0s\0e\0c\0t\0i\0o\0n\0",	28 },	/* CDATA section */
		{ NULL,			0 },	/* Entity reference */
		{ NULL,			0 },	/* Entity */
		{ NULL,			0 },	/* Processing instruction */
		{ "#\0c\0o\0m\0m\0e\0n\0t\0",		16 },	/* Comment */
		{ "#\0d\0o\0c\0u\0m\0e\0n\0t\0",		18 },	/* Document */
		{ NULL,			0 },	/* Document type */
		{ "#\0d\0o\0c\0u\0m\0e\0n\0t\0-\0f\0r\0a\0g\0m\0e\0n\0t\0",	36 },	/* Document fragment */
		{ NULL,			0 }	/* Notation */
	};

	dom_exception err;

	/* Initialise interned node names */
	for (int i = 0; i <= DOM_NODE_TYPE_COUNT; i++) {
		if (names_utf8[i].name == NULL) {
			/* Nothing to intern; skip this entry */
			__nodenames_utf8[i] = NULL;
			__nodenames_utf16[i] = NULL;
			continue;
		}

		/* Make string */
		err = dom_string_create_from_ptr_no_doc(alloc, pw,
				DOM_STRING_UTF8,
				(const uint8_t *) names_utf8[i].name,
				names_utf8[i].len, &__nodenames_utf8[i]);
		if (err != DOM_NO_ERR) {
			/* Failed, clean up strings we've created so far */
			for (int j = 0; j < i; j++) {
				if (__nodenames_utf8[j] != NULL) {
					dom_string_unref(__nodenames_utf8[j]);
					dom_string_unref(__nodenames_utf16[j]);
				}
			}
			return err;
		}

		err = dom_string_create_from_ptr_no_doc(alloc, pw,
				DOM_STRING_UTF16,
				(const uint8_t *) names_utf16[i].name,
				names_utf16[i].len, &__nodenames_utf16[i]);
		if (err != DOM_NO_ERR) {
			/* Failed, clean up strings we've created so far */
			for (int j = 0; j < i; j++) {
				if (__nodenames_utf8[j] != NULL) {
					dom_string_unref(__nodenames_utf8[j]);
					dom_string_unref(__nodenames_utf16[j]);
				}
			}

			dom_string_unref(__nodenames_utf8[i]);

			return err;
		}
	}

	return DOM_NO_ERR;
}

/**
 * Finalise the document module
 *
 * \return DOM_NO_ERR.
 */
dom_exception _dom_document_finalise(void)
{
	for (int i = 0; i <= DOM_NODE_TYPE_COUNT; i++) {
		if (__nodenames_utf8[i] != NULL) {
			dom_string_unref(__nodenames_utf8[i]);
			dom_string_unref(__nodenames_utf16[i]);
		}
	}

	return DOM_NO_ERR;
}

/**
 * Create a Document
 *
 * \param impl     The DOM implementation owning the document
 * \param charset  The charset used for strings in the document
 * \param alloc    Memory (de)allocation function
 * \param pw       Pointer to client-specific private data
 * \param doc      Pointer to location to receive created document
 * \return DOM_NO_ERR on success, DOM_NO_MEM_ERR on memory exhaustion.
 *
 * ::impl will have its reference count increased.
 *
 * The returned document will already be referenced.
 */
dom_exception dom_document_create(struct dom_implementation *impl,
		dom_string_charset charset, dom_alloc alloc, void *pw, 
		struct dom_document **doc)
{
	struct dom_document *d;
	dom_exception err;

	/* Create document */
	d = alloc(NULL, sizeof(struct dom_document), pw);
	if (d == NULL)
		return DOM_NO_MEM_ERR;

	/* Set up document allocation context - must be first */
	d->alloc = alloc;
	d->pw = pw;

	/* Initialise base class -- the Document has no parent, so
	 * destruction will be attempted as soon as its reference count
	 * reaches zero. Documents own themselves (this simplifies the 
	 * rest of the code, as it doesn't need to special case Documents)
	 */
	err = dom_node_initialise(&d->base, d, DOM_DOCUMENT_NODE,
			NULL, NULL, NULL, NULL);
	if (err != DOM_NO_ERR) {
		/* Clean up document */
		alloc(d, 0, pw);
		return err;
	}

	/* Initialise remaining type-specific data */
	d->charset = charset;
	if (impl != NULL)
		dom_implementation_ref(impl);
	d->impl = impl;

	d->nodelists = NULL;
	d->maps = NULL;

	d->nodenames = (charset == DOM_STRING_UTF8) ? __nodenames_utf8 
						    : __nodenames_utf16;

	*doc = d;

	return DOM_NO_ERR;
}

/**
 * Destroy a document
 *
 * \param doc  The document to destroy
 *
 * The contents of ::doc will be destroyed and ::doc will be freed.
 */
void dom_document_destroy(struct dom_document *doc)
{
	struct dom_node *c, *d;

	/* Destroy children of this node */
	for (c = doc->base.first_child; c != NULL; c = d) {
		d = c->next;

		/* Detach child */
		c->parent = NULL;

		if (c->refcnt > 0) {
			/* Something is using this child */

			/** \todo add to list of nodes pending deletion */

			continue;
		}

		/* Detach from sibling list */
		c->previous = NULL;
		c->next = NULL;

		dom_node_destroy(c);
	}

	/** \todo Ensure list of nodes pending deletion is empty. If not,
	 * then we can't yet destroy the document (its destruction will
	 * have to wait until the pending nodes are destroyed) */

	/* Ok, the document tree is empty, as is the list of nodes pending
	 * deletion. Therefore, it is safe to destroy the document. */

	if (doc->impl != NULL)
		dom_implementation_unref(doc->impl);
	doc->impl = NULL;

	/* This is paranoia -- if there are any remaining nodelists or
	 * namednodemaps, then the document's reference count will be
	 * non-zero as these data structures reference the document because
	 * they are held by the client. */
	doc->nodelists = NULL;
	doc->maps = NULL;

	/* Finalise base class */
	dom_node_finalise(doc, &doc->base);

	/* Free document */
	doc->alloc(doc, 0, doc->pw);
}

/**
 * Retrieve the doctype of a document
 *
 * \param doc     The document to retrieve the doctype from
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception dom_document_get_doctype(struct dom_document *doc,
		struct dom_document_type **result)
{
	struct dom_node *c;

	for (c = doc->base.first_child; c != NULL; c = c->next) {
		if (c->type == DOM_DOCUMENT_TYPE_NODE)
			break;
	}

	if (c != NULL)
		dom_node_ref(c);

	*result = (struct dom_document_type *) c;

	return DOM_NO_ERR;
}

/**
 * Retrieve the DOM implementation that handles this document
 *
 * \param doc     The document to retrieve the implementation from
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned implementation will have its reference count increased.
 * It is the responsibility of the caller to unref the implementation once
 * it has finished with it.
 */
dom_exception dom_document_get_implementation(struct dom_document *doc,
		struct dom_implementation **result)
{
	if (doc->impl != NULL)
		dom_implementation_ref(doc->impl);

	*result = doc->impl;

	return DOM_NO_ERR;
}

/**
 * Retrieve the document element of a document
 *
 * \param doc     The document to retrieve the document element from
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception dom_document_get_document_element(struct dom_document *doc,
		struct dom_element **result)
{
	struct dom_node *root;

	/* Find first element node in child list */
	for (root = doc->base.first_child; root != NULL; root = root->next) {
		if (root->type == DOM_ELEMENT_NODE)
			break;
	}

	if (root != NULL)
		dom_node_ref(root);

	*result = (struct dom_element *) root;

	return DOM_NO_ERR;
}

/**
 * Create an element
 *
 * \param doc       The document owning the element
 * \param tag_name  The name of the element
 * \param result    Pointer to location to receive result
 * \return DOM_NO_ERR                on success,
 *         DOM_INVALID_CHARACTER_ERR if ::tag_name is invalid.
 *
 * ::doc and ::tag_name will have their reference counts increased.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception dom_document_create_element(struct dom_document *doc,
		struct dom_string *tag_name, struct dom_element **result)
{
	return dom_element_create(doc, tag_name, NULL, NULL, result);
}

/**
 * Create a document fragment
 *
 * \param doc     The document owning the fragment
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception dom_document_create_document_fragment(struct dom_document *doc,
		struct dom_document_fragment **result)
{
	return dom_document_fragment_create(doc,
			doc->nodenames[DOM_DOCUMENT_FRAGMENT_NODE],
			NULL, result);
}

/**
 * Create a text node
 *
 * \param doc     The document owning the node
 * \param data    The data for the node
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception dom_document_create_text_node(struct dom_document *doc,
		struct dom_string *data, struct dom_text **result)
{
	return dom_text_create(doc, doc->nodenames[DOM_TEXT_NODE],
			data, result);
}

/**
 * Create a comment node
 *
 * \param doc     The document owning the node
 * \param data    The data for the node
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception dom_document_create_comment(struct dom_document *doc,
		struct dom_string *data, struct dom_comment **result)
{
	return dom_comment_create(doc, doc->nodenames[DOM_COMMENT_NODE],
			data, result);
}

/**
 * Create a CDATA section
 *
 * \param doc     The document owning the section
 * \param data    The data for the section contents
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR            on success,
 *         DOM_NOT_SUPPORTED_ERR if this is an HTML document.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception dom_document_create_cdata_section(struct dom_document *doc,
		struct dom_string *data, struct dom_cdata_section **result)
{
	return dom_cdata_section_create(doc,
			doc->nodenames[DOM_CDATA_SECTION_NODE],
			data, result);
}

/**
 * Create a processing instruction
 *
 * \param doc     The document owning the instruction
 * \param target  The instruction target
 * \param data    The data for the node
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR                on success,
 *         DOM_INVALID_CHARACTER_ERR if ::target is invalid,
 *         DOM_NOT_SUPPORTED_ERR     if this is an HTML document.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception dom_document_create_processing_instruction(
		struct dom_document *doc, struct dom_string *target,
		struct dom_string *data,
		struct dom_processing_instruction **result)
{
	return dom_processing_instruction_create(doc, target, data, result);
}

/**
 * Create an attribute
 *
 * \param doc     The document owning the attribute
 * \param name    The name of the attribute
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR                on success,
 *         DOM_INVALID_CHARACTER_ERR if ::name is invalid.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception dom_document_create_attribute(struct dom_document *doc,
		struct dom_string *name, struct dom_attr **result)
{
	return dom_attr_create(doc, name, NULL, NULL, result);
}

/**
 * Create an entity reference
 *
 * \param doc     The document owning the reference
 * \param name    The name of the entity to reference
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR                on success,
 *         DOM_INVALID_CHARACTER_ERR if ::name is invalid,
 *         DOM_NOT_SUPPORTED_ERR     if this is an HTML document.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception dom_document_create_entity_reference(struct dom_document *doc,
		struct dom_string *name,
		struct dom_entity_reference **result)
{
	return dom_entity_reference_create(doc, name, NULL, result);
}

/**
 * Retrieve a list of all elements with a given tag name
 *
 * \param doc      The document to search in
 * \param tagname  The tag name to search for ("*" for all)
 * \param result   Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned list will have its reference count increased. It is
 * the responsibility of the caller to unref the list once it has
 * finished with it.
 */
dom_exception dom_document_get_elements_by_tag_name(struct dom_document *doc,
		struct dom_string *tagname, struct dom_nodelist **result)
{
	return dom_document_get_nodelist(doc, (struct dom_node *) doc, 
			tagname, NULL, NULL, result);
}

/**
 * Import a node from another document into this one
 *
 * \param doc     The document to import into
 * \param node    The node to import
 * \param deep    Whether to copy the node's subtree
 * \param result  Pointer to location to receive imported node in this document.
 * \return DOM_NO_ERR                on success,
 *         DOM_INVALID_CHARACTER_ERR if any of the names are invalid,
 *         DOM_NOT_SUPPORTED_ERR     if the type of ::node is unsupported
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception dom_document_import_node(struct dom_document *doc,
		struct dom_node *node, bool deep, struct dom_node **result)
{
	UNUSED(doc);
	UNUSED(node);
	UNUSED(deep);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Create an element from the qualified name and namespace URI
 *
 * \param doc        The document owning the element
 * \param namespace  The namespace URI to use, or NULL for none
 * \param qname      The qualified name of the element
 * \param result     Pointer to location to receive result
 * \return DOM_NO_ERR                on success,
 *         DOM_INVALID_CHARACTER_ERR if ::qname is invalid,
 *         DOM_NAMESPACE_ERR         if ::qname is malformed, or it has a
 *                                   prefix and ::namespace is NULL, or
 *                                   ::qname has a prefix "xml" and
 *                                   ::namespace is not
 *                                   "http://www.w3.org/XML/1998/namespace",
 *                                   or ::qname has a prefix "xmlns" and
 *                                   ::namespace is not
 *                                   "http://www.w3.org/2000/xmlns", or
 *                                   ::namespace is
 *                                   "http://www.w3.org/2000/xmlns" and
 *                                   ::qname is not (or is not prefixed by)
 *                                   "xmlns",
 *         DOM_NOT_SUPPORTED_ERR     if ::doc does not support the "XML"
 *                                   feature.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception dom_document_create_element_ns(struct dom_document *doc,
		struct dom_string *namespace, struct dom_string *qname,
		struct dom_element **result)
{
	struct dom_string *prefix, *localname;
	dom_exception err;

	/** \todo ensure document supports XML feature */

	/* Validate qname */
	err = _dom_namespace_validate_qname(qname, namespace);
	if (err != DOM_NO_ERR) {
		return err;
	}

	/* Divide QName into prefix/localname pair */
	err = _dom_namespace_split_qname(qname, &prefix, &localname);
	if (err != DOM_NO_ERR) {
		return err;
	}

	/* Attempt to create element */
	err = dom_element_create(doc, localname, namespace, prefix, result);

	/* Tidy up */
	dom_string_unref(localname);
	if (prefix != NULL) {
		dom_string_unref(prefix);
	}

	return err;
}

/**
 * Create an attribute from the qualified name and namespace URI
 *
 * \param doc        The document owning the attribute
 * \param namespace  The namespace URI to use
 * \param qname      The qualified name of the attribute
 * \param result     Pointer to location to receive result
 * \return DOM_NO_ERR                on success,
 *         DOM_INVALID_CHARACTER_ERR if ::qname is invalid,
 *         DOM_NAMESPACE_ERR         if ::qname is malformed, or it has a
 *                                   prefix and ::namespace is NULL, or
 *                                   ::qname has a prefix "xml" and
 *                                   ::namespace is not
 *                                   "http://www.w3.org/XML/1998/namespace",
 *                                   or ::qname has a prefix "xmlns" and
 *                                   ::namespace is not
 *                                   "http://www.w3.org/2000/xmlns", or
 *                                   ::namespace is
 *                                   "http://www.w3.org/2000/xmlns" and
 *                                   ::qname is not (or is not prefixed by)
 *                                   "xmlns",
 *         DOM_NOT_SUPPORTED_ERR     if ::doc does not support the "XML"
 *                                   feature.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception dom_document_create_attribute_ns(struct dom_document *doc,
		struct dom_string *namespace, struct dom_string *qname,
		struct dom_attr **result)
{
	struct dom_string *prefix, *localname;
	dom_exception err;

	/** \todo ensure document supports XML feature */

	/* Validate qname */
	err = _dom_namespace_validate_qname(qname, namespace);
	if (err != DOM_NO_ERR) {
		return err;
	}

	/* Divide QName into prefix/localname pair */
	err = _dom_namespace_split_qname(qname, &prefix, &localname);
	if (err != DOM_NO_ERR) {
		return err;
	}

	/* Attempt to create attribute */
	err = dom_attr_create(doc, localname, namespace, prefix, result);

	/* Tidy up */
	dom_string_unref(localname);
	if (prefix != NULL) {
		dom_string_unref(prefix);
	}

	return err;
}

/**
 * Retrieve a list of all elements with a given local name and namespace URI
 *
 * \param doc        The document to search in
 * \param namespace  The namespace URI
 * \param localname  The local name
 * \param result     Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned list will have its reference count increased. It is
 * the responsibility of the caller to unref the list once it has
 * finished with it.
 */
dom_exception dom_document_get_elements_by_tag_name_ns(
		struct dom_document *doc, struct dom_string *namespace,
		struct dom_string *localname, struct dom_nodelist **result)
{
	return dom_document_get_nodelist(doc, (struct dom_node *) doc, 
			NULL, namespace, localname, result);
}

/**
 * Retrieve the element that matches the specified ID
 *
 * \param doc     The document to search in
 * \param id      The ID to search for
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception dom_document_get_element_by_id(struct dom_document *doc,
		struct dom_string *id, struct dom_element **result)
{
	UNUSED(doc);
	UNUSED(id);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Retrieve the input encoding of the document
 *
 * \param doc     The document to query
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception dom_document_get_input_encoding(struct dom_document *doc,
		struct dom_string **result)
{
	UNUSED(doc);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Retrieve the XML encoding of the document
 *
 * \param doc     The document to query
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception dom_document_get_xml_encoding(struct dom_document *doc,
		struct dom_string **result)
{
	UNUSED(doc);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Retrieve the standalone status of the document
 *
 * \param doc     The document to query
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 */
dom_exception dom_document_get_xml_standalone(struct dom_document *doc,
		bool *result)
{
	UNUSED(doc);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Set the standalone status of the document
 *
 * \param doc         The document to query
 * \param standalone  Standalone status to use
 * \return DOM_NO_ERR            on success,
 *         DOM_NOT_SUPPORTED_ERR if the document does not support the "XML"
 *                               feature.
 */
dom_exception dom_document_set_xml_standalone(struct dom_document *doc,
		bool standalone)
{
	UNUSED(doc);
	UNUSED(standalone);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Retrieve the XML version of the document
 *
 * \param doc     The document to query
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception dom_document_get_xml_version(struct dom_document *doc,
		struct dom_string **result)
{
	UNUSED(doc);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Set the XML version of the document
 *
 * \param doc      The document to query
 * \param version  XML version to use
 * \return DOM_NO_ERR            on success,
 *         DOM_NOT_SUPPORTED_ERR if the document does not support the "XML"
 *                               feature.
 */
dom_exception dom_document_set_xml_version(struct dom_document *doc,
		struct dom_string *version)
{
	UNUSED(doc);
	UNUSED(version);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Retrieve the error checking mode of the document
 *
 * \param doc     The document to query
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 */
dom_exception dom_document_get_strict_error_checking(
		struct dom_document *doc, bool *result)
{
	UNUSED(doc);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Set the error checking mode of the document
 *
 * \param doc     The document to query
 * \param strict  Whether to use strict error checking
 * \return DOM_NO_ERR.
 */
dom_exception dom_document_set_strict_error_checking(
		struct dom_document *doc, bool strict)
{
	UNUSED(doc);
	UNUSED(strict);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Retrieve the URI of the document
 *
 * \param doc     The document to query
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception dom_document_get_uri(struct dom_document *doc,
		struct dom_string **result)
{
	UNUSED(doc);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Set the URI of the document
 *
 * \param doc  The document to query
 * \param uri  The URI to use
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception dom_document_set_uri(struct dom_document *doc,
		struct dom_string *uri)
{
	UNUSED(doc);
	UNUSED(uri);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Attempt to adopt a node from another document into this document
 *
 * \param doc     The document to adopt into
 * \param node    The node to adopt
 * \param result  Pointer to location to receive adopted node
 * \return DOM_NO_ERR                      on success,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if ::node is readonly,
 *         DOM_NOT_SUPPORTED_ERR           if ::node is of type Document or
 *                                         DocumentType
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception dom_document_adopt_node(struct dom_document *doc,
		struct dom_node *node, struct dom_node **result)
{
	UNUSED(doc);
	UNUSED(node);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Retrieve the DOM configuration associated with a document
 *
 * \param doc     The document to query
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned object will have its reference count increased. It is
 * the responsibility of the caller to unref the object once it has
 * finished with it.
 */
dom_exception dom_document_get_dom_config(struct dom_document *doc,
		struct dom_configuration **result)
{
	UNUSED(doc);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Normalize a document
 *
 * \param doc  The document to normalize
 * \return DOM_NO_ERR.
 */
dom_exception dom_document_normalize(struct dom_document *doc)
{
	UNUSED(doc);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Rename a node in a document
 *
 * \param doc        The document containing the node
 * \param node       The node to rename
 * \param namespace  The new namespace for the node
 * \param qname      The new qualified name for the node
 * \param result     Pointer to location to receive renamed node
 * \return DOM_NO_ERR                on success,
 *         DOM_INVALID_CHARACTER_ERR if ::tag_name is invalid,
 *         DOM_WRONG_DOCUMENT_ERR    if ::node was created in a different
 *                                   document
 *         DOM_NAMESPACE_ERR         if ::qname is malformed, or it has a
 *                                   prefix and ::namespace is NULL, or
 *                                   ::qname has a prefix "xml" and
 *                                   ::namespace is not
 *                                   "http://www.w3.org/XML/1998/namespace",
 *                                   or ::qname has a prefix "xmlns" and
 *                                   ::namespace is not
 *                                   "http://www.w3.org/2000/xmlns", or
 *                                   ::namespace is
 *                                   "http://www.w3.org/2000/xmlns" and
 *                                   ::qname is not (or is not prefixed by)
 *                                   "xmlns",
 *         DOM_NOT_SUPPORTED_ERR     if ::doc does not support the "XML"
 *                                   feature.
 *
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception dom_document_rename_node(struct dom_document *doc,
		struct dom_node *node,
		struct dom_string *namespace, struct dom_string *qname,
		struct dom_node **result)
{
	UNUSED(doc);
	UNUSED(node);
	UNUSED(namespace);
	UNUSED(qname);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/*                                                                         */
/* ----------------------------------------------------------------------- */
/*                                                                         */

/**
 * Acquire a pointer to the base of the document buffer
 *
 * \param doc  Document to retrieve pointer from
 * \return Pointer to document buffer
 *
 * The document buffer is _not_ reference counted (as it is an implicit part
 * of the document). It is destroyed with the document, and thus after all
 * users have been destroyed.
 */
const uint8_t *dom_document_get_base(struct dom_document *doc)
{
	UNUSED(doc);

	return NULL;
}

/**
 * Set the document buffer pointer
 *
 * \param doc         Document to set buffer pointer of
 * \param buffer      Pointer to buffer
 * \param buffer_len  Length of buffer, in bytes
 *
 * By calling this, ownership of the buffer is transferred to the document.
 * It should be called once per document node.
 */
void dom_document_set_buffer(struct dom_document *doc, uint8_t *buffer,
		size_t buffer_len)
{
	UNUSED(doc);
	UNUSED(buffer);
	UNUSED(buffer_len);
}

/**
 * Retrieve the character set used to encode strings in the document
 *
 * \param doc  The document to get the charset of
 * \return The charset in use
 */
dom_string_charset dom_document_get_charset(struct dom_document *doc)
{
	return doc->charset;
}

/**
 * (De)allocate memory with a document's context
 *
 * \param doc   The document context to allocate from
 * \param ptr   Pointer to data to reallocate, or NULL to alloc new
 * \param size  Required size of data, or 0 to free
 * \return Pointer to allocated data or NULL on failure
 *
 * This call (modulo ::doc) has the same semantics as realloc().
 * It is a thin veneer over the client-provided allocation function.
 */
void *dom_document_alloc(struct dom_document *doc, void *ptr, size_t size)
{
	return doc->alloc(ptr, size, doc->pw);
}

/**
 * Get a nodelist, creating one if necessary
 *
 * \param doc        The document to get a nodelist for
 * \param root       Root node of subtree that list applies to
 * \param tagname    Name of nodes in list (or NULL)
 * \param namespace  Namespace part of nodes in list (or NULL)
 * \param localname  Local part of nodes in list (or NULL)
 * \param list       Pointer to location to receive list
 * \return DOM_NO_ERR on success, DOM_NO_MEM_ERR on memory exhaustion
 *
 * The returned list will have its reference count increased. It is
 * the responsibility of the caller to unref the list once it has
 * finished with it.
 */
dom_exception dom_document_get_nodelist(struct dom_document *doc,
		struct dom_node *root, struct dom_string *tagname,
		struct dom_string *namespace, struct dom_string *localname,
		struct dom_nodelist **list)
{
	struct dom_doc_nl *l;
	dom_exception err;

	for (l = doc->nodelists; l; l = l->next) {
		if (dom_nodelist_match(l->list, root, tagname,
				namespace, localname))
			break;
	}

	if (l != NULL) {
		/* Found an existing list, so use it */
		dom_nodelist_ref(l->list);
	} else {
		/* No existing list */

		/* Create active list entry */
		l = doc->alloc(NULL, sizeof(struct dom_doc_nl), doc->pw);
		if (l == NULL)
			return DOM_NO_MEM_ERR;

		/* Create nodelist */
		err = dom_nodelist_create(doc, root, tagname, namespace,
				localname, &l->list);
		if (err != DOM_NO_ERR) {
			doc->alloc(l, 0, doc->pw);
			return err;
		}

		/* Add to document's list of active nodelists */
		l->prev = NULL;
		l->next = doc->nodelists;
		if (doc->nodelists)
			doc->nodelists->prev = l;
		doc->nodelists = l;
	}

	/* Note: the document does not claim a reference on the nodelist
	 * If it did, the nodelist's reference count would never reach zero,
	 * and the list would remain indefinitely. This is not a problem as
	 * the list notifies the document of its destruction via
	 * dom_document_remove_nodelist. */

	*list = l->list;

	return DOM_NO_ERR;
}

/**
 * Remove a nodelist from a document
 *
 * \param doc   The document to remove the list from
 * \param list  The list to remove
 */
void dom_document_remove_nodelist(struct dom_document *doc,
		struct dom_nodelist *list)
{
	struct dom_doc_nl *l;

	for (l = doc->nodelists; l; l = l->next) {
		if (l->list == list)
			break;
	}

	if (l == NULL) {
		/* This should never happen; we should probably abort here */
		return;
	}

	/* Remove from list */
	if (l->prev != NULL)
		l->prev->next = l->next;
	else
		doc->nodelists = l->next;

	if (l->next != NULL)
		l->next->prev = l->prev;

	/* And free item */
	doc->alloc(l, 0, doc->pw);
}

/**
 * Get a namednodemap, creating one if necessary
 *
 * \param doc   The document to get a namednodemap for
 * \param head  Start of list containing items in map
 * \param type  The type of items in map
 * \param map   Pointer to location to receive map
 * \return DOM_NO_ERR on success, DOM_NO_MEM_ERR on memory exhaustion.
 *
 * The returned map will have its reference count increased. It is
 * the responsibility of the caller to unref the map once it has
 * finished with it.
 */
dom_exception dom_document_get_namednodemap(struct dom_document *doc,
		struct dom_node *head, dom_node_type type,
		struct dom_namednodemap **map)
{
	struct dom_doc_nnm *m;
	dom_exception err;

	for (m = doc->maps; m; m = m->next) {
		if (dom_namednodemap_match(m->map, head, type))
			break;
	}

	if (m != NULL) {
		/* Found an existing map, so use it */
		dom_namednodemap_ref(m->map);
	} else {
		/* No existing map */

		/* Create active map entry */
		m = doc->alloc(NULL, sizeof(struct dom_doc_nnm), doc->pw);
		if (m == NULL)
			return DOM_NO_MEM_ERR;

		/* Create namednodemap */
		err = dom_namednodemap_create(doc, head, type, &m->map);
		if (err != DOM_NO_ERR) {
			doc->alloc(m, 0, doc->pw);
			return err;
		}

		/* Add to document's list of active namednodemaps */
		m->prev = NULL;
		m->next = doc->maps;
		if (doc->maps)
			doc->maps->prev = m;
		doc->maps = m;
	}

	/* Note: the document does not claim a reference on the namednodemap
	 * If it did, the map's reference count would never reach zero,
	 * and the list would remain indefinitely. This is not a problem as
	 * the map notifies the document of its destruction via
	 * dom_document_remove_namednodempa. */

	*map = m->map;

	return DOM_NO_ERR;
}

/**
 * Remove a namednodemap
 *
 * \param doc  The document to remove the map from
 * \param map  The map to remove
 */
void dom_document_remove_namednodemap(struct dom_document *doc,
		struct dom_namednodemap *map)
{
	struct dom_doc_nnm *m;

	for (m = doc->maps; m; m = m->next) {
		if (m->map == map)
			break;
	}

	if (m == NULL) {
		/* This should never happen; we should probably abort here */
		return;
	}

	/* Remove from list */
	if (m->prev != NULL)
		m->prev->next = m->next;
	else
		doc->maps = m->next;

	if (m->next != NULL)
		m->next->prev = m->prev;

	/* And free item */
	doc->alloc(m, 0, doc->pw);
}
