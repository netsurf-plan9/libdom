/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_internal_core_document_h_
#define dom_internal_core_document_h_

#include <inttypes.h>
#include <stddef.h>

#include <dom/core/node.h>
#include <dom/core/string.h>

#include "core/node.h"

struct dom_document;
struct dom_namednodemap;
struct dom_node;
struct dom_nodelist;
struct dom_document_type;
struct dom_element;
struct dom_document_fragment;
struct dom_text;
struct dom_comment;
struct dom_cdata_section;
struct dom_processing_instruction;
struct dom_attr;
struct dom_entity_reference;
struct dom_configuration;

struct dom_doc_nl;
struct dom_doc_nnm;

/**
 * DOM document
 * This should be protected, because later the HTMLDocument will inherit from
 * this. 
 */
struct dom_document {
	struct dom_node_internal base;		/**< Base node */

	struct dom_implementation *impl;	/**< Owning implementation */

	struct dom_doc_nl *nodelists;	/**< List of active nodelists */

	struct dom_doc_nnm *maps;	/**< List of active namednodemaps */

	struct dom_string **nodenames;	/**< Interned nodenames */

	dom_alloc alloc;		/**< Memory (de)allocation function */
	void *pw;			/**< Pointer to client data */
};

dom_exception _dom_document_get_doctype(struct dom_document *doc,
		struct dom_document_type **result);
dom_exception _dom_document_get_implementation(struct dom_document *doc,
		struct dom_implementation **result);
dom_exception _dom_document_get_document_element(struct dom_document *doc,
		struct dom_element **result);
dom_exception _dom_document_create_element(struct dom_document *doc,
		struct dom_string *tag_name, struct dom_element **result);
dom_exception _dom_document_create_document_fragment(struct dom_document *doc,
		struct dom_document_fragment **result);
dom_exception _dom_document_create_text_node(struct dom_document *doc,
		struct dom_string *data, struct dom_text **result);
dom_exception _dom_document_create_comment(struct dom_document *doc,
		struct dom_string *data, struct dom_comment **result);
dom_exception _dom_document_create_cdata_section(struct dom_document *doc,
		struct dom_string *data, struct dom_cdata_section **result);
dom_exception _dom_document_create_processing_instruction(
		struct dom_document *doc, struct dom_string *target,
		struct dom_string *data,
		struct dom_processing_instruction **result);
dom_exception _dom_document_create_attribute(struct dom_document *doc,
		struct dom_string *name, struct dom_attr **result);
dom_exception _dom_document_create_entity_reference(struct dom_document *doc,
		struct dom_string *name,
		struct dom_entity_reference **result);
dom_exception _dom_document_get_elements_by_tag_name(struct dom_document *doc,
		struct dom_string *tagname, struct dom_nodelist **result);
dom_exception _dom_document_import_node(struct dom_document *doc,
		struct dom_node *node, bool deep, struct dom_node **result);
dom_exception _dom_document_create_element_ns(struct dom_document *doc,
		struct dom_string *namespace, struct dom_string *qname,
		struct dom_element **result);
dom_exception _dom_document_create_attribute_ns(struct dom_document *doc,
		struct dom_string *namespace, struct dom_string *qname,
		struct dom_attr **result);
dom_exception _dom_document_get_elements_by_tag_name_ns(
		struct dom_document *doc, struct dom_string *namespace,
		struct dom_string *localname, struct dom_nodelist **result);
dom_exception _dom_document_get_element_by_id(struct dom_document *doc,
		struct dom_string *id, struct dom_element **result);
dom_exception _dom_document_get_input_encoding(struct dom_document *doc,
		struct dom_string **result);
dom_exception _dom_document_get_xml_encoding(struct dom_document *doc,
		struct dom_string **result);
dom_exception _dom_document_get_xml_standalone(struct dom_document *doc,
		bool *result);
dom_exception _dom_document_set_xml_standalone(struct dom_document *doc,
		bool standalone);
dom_exception _dom_document_get_xml_version(struct dom_document *doc,
		struct dom_string **result);
dom_exception _dom_document_set_xml_version(struct dom_document *doc,
		struct dom_string *version);
dom_exception _dom_document_get_strict_error_checking(
		struct dom_document *doc, bool *result);
dom_exception _dom_document_set_strict_error_checking(
		struct dom_document *doc, bool strict);
dom_exception _dom_document_get_uri(struct dom_document *doc,
		struct dom_string **result);
dom_exception _dom_document_set_uri(struct dom_document *doc,
		struct dom_string *uri);
dom_exception _dom_document_adopt_node(struct dom_document *doc,
		struct dom_node *node, struct dom_node **result);
dom_exception _dom_document_get_dom_config(struct dom_document *doc,
		struct dom_configuration **result);
dom_exception _dom_document_normalize(struct dom_document *doc);
dom_exception _dom_document_rename_node(struct dom_document *doc,
		struct dom_node *node,
		struct dom_string *namespace, struct dom_string *qname,
		struct dom_node **result);

#define DOM_DOCUMENT_VTABLE \
	_dom_document_get_doctype, \
	_dom_document_get_implementation, \
	_dom_document_get_document_element, \
	_dom_document_create_element, \
	_dom_document_create_document_fragment, \
	_dom_document_create_text_node, \
	_dom_document_create_comment, \
	_dom_document_create_cdata_section, \
	_dom_document_create_processing_instruction, \
	_dom_document_create_attribute, \
	_dom_document_create_entity_reference, \
	_dom_document_get_elements_by_tag_name, \
	_dom_document_import_node, \
	_dom_document_create_element_ns, \
	_dom_document_create_attribute_ns, \
	_dom_document_get_elements_by_tag_name_ns, \
	_dom_document_get_element_by_id, \
	_dom_document_get_input_encoding, \
	_dom_document_get_xml_encoding, \
	_dom_document_get_xml_standalone, \
	_dom_document_set_xml_standalone, \
	_dom_document_get_xml_version, \
	_dom_document_set_xml_version, \
	_dom_document_get_strict_error_checking, \
	_dom_document_set_strict_error_checking, \
	_dom_document_get_uri, \
	_dom_document_set_uri, \
	_dom_document_adopt_node, \
	_dom_document_get_dom_config, \
	_dom_document_normalize, \
	_dom_document_rename_node


/* Initialise the document module */
dom_exception _dom_document_initialise(dom_alloc alloc, void *pw);
/* Finalise the document module */
dom_exception _dom_document_finalise(void);

/* Destroy a document */
void dom_document_destroy(struct dom_node_internal *dnode);

/* (De)allocate memory */
void *dom_document_alloc(struct dom_document *doc, void *ptr, size_t size);

/* Get a nodelist, creating one if necessary */
dom_exception dom_document_get_nodelist(struct dom_document *doc,
		struct dom_node *root, struct dom_string *tagname,
		struct dom_string *namespace, struct dom_string *localname,
		struct dom_nodelist **list);
/* Remove a nodelist */
void dom_document_remove_nodelist(struct dom_document *doc,
		struct dom_nodelist *list);

/* Get a namednodemap, creating one if necessary */
dom_exception dom_document_get_namednodemap(struct dom_document *doc,
		struct dom_node *head, dom_node_type type,
		struct dom_namednodemap **map);
/* Remove a namednodemap */
void dom_document_remove_namednodemap(struct dom_document *doc,
		struct dom_namednodemap *map);

#endif
