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
#include <dom/core/document.h>

#include "core/string.h"
#include "core/node.h"
#include "core/nodelist.h"

#include "utils/hashtable.h"
#include "utils/resource_mgr.h"
#include "utils/list.h"

#include "events/document_event.h"

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

/**
 * DOM document
 * This should be protected, because later the HTMLDocument will inherit from
 * this. 
 */
struct dom_document {
	struct dom_node_internal base;		/**< Base node */

	struct dom_implementation *impl;	/**< Owning implementation */

	struct dom_doc_nl *nodelists;	/**< List of active nodelists */

	struct dom_string *uri;		/**< The uri of this document */

	dom_alloc alloc;		/**< Memory (de)allocation function */
	void *pw;			/**< Pointer to client data */

	struct list_entry pending_nodes;
			/**< The deletion pending list */

	struct lwc_string_s *id_name;	/**< The ID attribute's name */

	dom_document_event_internal dei;
			/**< The DocumentEVent interface */
};

/* Create a DOM document */
dom_exception _dom_document_create(struct dom_implementation *impl,
		dom_alloc alloc, void *pw,
		dom_events_default_action_fetcher daf,
		struct dom_document **doc);

/* Initialise the document */
dom_exception _dom_document_initialise(struct dom_document *doc, 
		struct dom_implementation *impl, dom_alloc alloc, void *pw, 
		dom_events_default_action_fetcher daf);

/* Finalise the document */
bool _dom_document_finalise(struct dom_document *doc);

/* Create a dom_string from C string */
dom_exception _dom_document_create_string(struct dom_document *doc,
		const uint8_t *data, size_t len, struct dom_string **result);
/* Create a lwc_string from C string */
dom_exception _dom_document_create_lwcstring(struct dom_document *doc,
		const uint8_t *data, size_t len, struct lwc_string_s **result);
/* Unref a lwc_string of this document */
void _dom_document_unref_lwcstring(struct dom_document *doc,
		struct lwc_string_s *str);
/* Create a dom_string from a lwc_string */
dom_exception _dom_document_create_string_from_lwcstring(
		struct dom_document *doc, struct lwc_string_s *str,
		struct dom_string **result);


/* Begin the virtual functions */
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
/* End of vtable */

/**
 * The internal used vtable for document
 */
struct dom_document_protected_vtable {
	struct dom_node_protect_vtable base;
	dom_exception (*dom_document_get_base)(dom_document *doc,
			struct dom_string **base_uri);
			/* Get the document's base uri */
};

typedef struct dom_document_protected_vtable dom_document_protected_vtable;

/* Get the document's base URI */
static inline dom_exception dom_document_get_base(dom_document *doc,
		struct dom_string **base_uri)
{
	struct dom_node_internal *node = (struct dom_node_internal *) doc;
	return ((dom_document_protected_vtable *) node->vtable)->
			dom_document_get_base(doc, base_uri);
}
#define dom_document_get_base(d, b) dom_document_get_base( \
		(dom_document *) (d), (struct dom_string **) (b))

/* Following comes the protected vtable  */
void _dom_document_destroy(struct dom_node_internal *node);
dom_exception __dom_document_alloc(struct dom_document *doc,
		struct dom_node_internal *n, struct dom_node_internal **ret);
dom_exception _dom_document_copy(struct dom_node_internal *new, 
		struct dom_node_internal *old);

#define DOM_DOCUMENT_PROTECT_VTABLE \
	_dom_document_destroy, \
	__dom_document_alloc, \
	_dom_document_copy


/*---------------------------- Helper functions ---------------------------*/

/* Try to destroy the document:
 * When the refcnt is zero and the pending list is empty, we can destroy this
 * document. */
void _dom_document_try_destroy(struct dom_document *doc);
/* (De)allocate memory */
void *_dom_document_alloc(struct dom_document *doc, void *ptr, size_t size);

/* Get the resource manager inside this document, a resource manager
 * is an object which contain the memory allocator/intern string context,
 * with which we can allocate strings or intern strings */
void _dom_document_get_resource_mgr(
		struct dom_document *doc, struct dom_resource_mgr *rm);

/* Get the internal allocator and its pointer */
void _dom_document_get_allocator(struct dom_document *doc, 
		dom_alloc *al, void **pw);

/* Create a hash_table */
dom_exception _dom_document_create_hashtable(struct dom_document *doc,
		size_t chains, dom_hash_func f, struct dom_hash_table **ht);

/* Get a nodelist, creating one if necessary */
dom_exception _dom_document_get_nodelist(struct dom_document *doc,
		nodelist_type type, struct dom_node_internal *root,
		struct lwc_string_s *tagname, struct lwc_string_s *namespace,
		struct lwc_string_s *localname, struct dom_nodelist **list);
/* Remove a nodelist */
void _dom_document_remove_nodelist(struct dom_document *doc,
		struct dom_nodelist *list);

/* Find element with certain ID in the subtree rooted at root */
dom_exception _dom_find_element_by_id(dom_node_internal *root, 
		struct lwc_string_s *id, struct dom_element **result);

/* Set the ID attribute name of this document */
void _dom_document_set_id_name(struct dom_document *doc,
		struct lwc_string_s *name);

#define _dom_document_get_id_name(d) (d->id_name)

#endif
