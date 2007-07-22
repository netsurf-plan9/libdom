/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef dom_core_document_h_
#define dom_core_document_h_

#include <stdbool.h>

#include <dom/core/exceptions.h>

struct dom_attr;
struct dom_characterdata;
struct dom_configuration;
struct dom_document;
struct dom_document_type;
struct dom_element;
struct dom_implementation;
struct dom_node;
struct dom_nodelist;
struct dom_string;
struct dom_text;

dom_exception dom_document_get_doctype(struct dom_document *doc,
		struct dom_document_type **result);
dom_exception dom_document_get_implementation(struct dom_document *doc,
		struct dom_implementation **result);
dom_exception dom_document_get_document_element(struct dom_document *doc,
		struct dom_element **result);
dom_exception dom_document_create_element(struct dom_document *doc,
		struct dom_string *tag_name, struct dom_element **result);
dom_exception dom_document_create_document_fragment(struct dom_document *doc,
		struct dom_node **result);
dom_exception dom_document_create_text_node(struct dom_document *doc,
		struct dom_string *data, struct dom_text **result);
dom_exception dom_document_create_comment(struct dom_document *doc,
		struct dom_string *data, struct dom_characterdata **result);
dom_exception dom_document_create_cdata_section(struct dom_document *doc,
		struct dom_string *data, struct dom_text **result);
dom_exception dom_document_create_processing_instruction(
		struct dom_document *doc, struct dom_string *target,
		struct dom_string *data, struct dom_node **result);
dom_exception dom_document_create_attribute(struct dom_document *doc,
		struct dom_string *name, struct dom_attr **result);
dom_exception dom_document_create_entity_reference(struct dom_document *doc,
		struct dom_string *name, struct dom_node **result);
dom_exception dom_document_get_elements_by_tag_name(struct dom_document *doc,
		struct dom_string *tagname, struct dom_nodelist **result);
dom_exception dom_document_import_node(struct dom_document *doc,
		struct dom_node *node, bool deep, struct dom_node **result);
dom_exception dom_document_create_element_ns(struct dom_document *doc,
		struct dom_string *namespace, struct dom_string *qname,
		struct dom_element **result);
dom_exception dom_document_create_attribute_ns(struct dom_document *doc,
		struct dom_string *namespace, struct dom_string *qname,
		struct dom_attr **result);
dom_exception dom_document_get_elements_by_tag_name_ns(
		struct dom_document *doc, struct dom_string *namespace,
		struct dom_string *localname, struct dom_nodelist **result);
dom_exception dom_document_get_element_by_id(struct dom_document *doc,
		struct dom_string *id, struct dom_element **result);
dom_exception dom_document_get_input_encoding(struct dom_document *doc,
		struct dom_string **result);
dom_exception dom_document_get_xml_encoding(struct dom_document *doc,
		struct dom_string **result);
dom_exception dom_document_get_xml_standalone(struct dom_document *doc,
		bool *result);
dom_exception dom_document_set_xml_standalone(struct dom_document *doc,
		bool standalone);
dom_exception dom_document_get_xml_version(struct dom_document *doc,
		struct dom_string **result);
dom_exception dom_document_set_xml_version(struct dom_document *doc,
		struct dom_string *version);
dom_exception dom_document_get_strict_error_checking(
		struct dom_document *doc, bool *result);
dom_exception dom_document_set_strict_error_checking(
		struct dom_document *doc, bool strict);
dom_exception dom_document_get_uri(struct dom_document *doc,
		struct dom_string **result);
dom_exception dom_document_set_uri(struct dom_document *doc,
		struct dom_string *uri);
dom_exception dom_document_adopt_node(struct dom_document *doc,
		struct dom_node *node, struct dom_node **result);
dom_exception dom_document_get_dom_config(struct dom_document *doc,
		struct dom_configuration **result);
dom_exception dom_document_normalize(struct dom_document *doc);
dom_exception dom_document_rename_node(struct dom_document *doc,
		struct dom_node *node,
		struct dom_string *namespace, struct dom_string *qname,
		struct dom_node **result);

#endif
