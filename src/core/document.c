/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#include <dom/functypes.h>
#include <dom/core/document.h>

#include "core/document.h"
#include "core/node.h"
#include "utils/utils.h"

/**
 * DOM document
 */
struct dom_document {
	struct dom_node base;		/**< Base node */

	dom_alloc alloc;		/**< Memory (de)allocation function */
	void *pw;			/**< Pointer to client data */
};

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
	UNUSED(doc);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Retrieve the DOM implementation that handles this document
 *
 * \param doc     The document to retrieve the implementation from
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 */
dom_exception dom_document_get_implementation(struct dom_document *doc,
		struct dom_implementation **result)
{
	UNUSED(doc);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
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
dom_exception dom_document_get_element(struct dom_document *doc,
		struct dom_element **result)
{
	UNUSED(doc);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
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
 * The returned node will have its reference count increased. It is
 * the responsibility of the caller to unref the node once it has
 * finished with it.
 */
dom_exception dom_document_create_element(struct dom_document *doc,
		struct dom_string *tag_name, struct dom_element **result)
{
	UNUSED(doc);
	UNUSED(tag_name);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
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
		struct dom_node **result)
{
	UNUSED(doc);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
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
	UNUSED(doc);
	UNUSED(data);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
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
		struct dom_string *data, struct dom_text **result)
{
	UNUSED(doc);
	UNUSED(data);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
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
		struct dom_node **result)
{
	UNUSED(doc);
	UNUSED(target);
	UNUSED(data);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
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
	UNUSED(doc);
	UNUSED(name);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
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
		struct dom_string *name, struct dom_node **result)
{
	UNUSED(doc);
	UNUSED(name);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
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
	UNUSED(doc);
	UNUSED(tagname);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
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
 * \param namespace  The namespace URI to use
 * \param qname      The qualified name of the element
 * \param result     Pointer to location to receive result
 * \return DOM_NO_ERR                on success,
 *         DOM_INVALID_CHARACTER_ERR if ::tag_name is invalid,
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
	UNUSED(doc);
	UNUSED(namespace);
	UNUSED(qname);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Create an attribute from the qualified name and namespace URI
 *
 * \param doc        The document owning the attribute
 * \param namespace  The namespace URI to use
 * \param qname      The qualified name of the attribute
 * \param result     Pointer to location to receive result
 * \return DOM_NO_ERR                on success,
 *         DOM_INVALID_CHARACTER_ERR if ::tag_name is invalid,
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
	UNUSED(doc);
	UNUSED(namespace);
	UNUSED(qname);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
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
	UNUSED(doc);
	UNUSED(namespace);
	UNUSED(localname);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
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
