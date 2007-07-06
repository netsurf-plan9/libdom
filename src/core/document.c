/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#include <dom/core/document.h>

#include "core/node.h"
#include "utils/utils.h"

/**
 * DOM document
 */
struct dom_document {
	struct dom_node base;		/**< Base node */
};


dom_exception dom_document_get_doctype(struct dom_ctx *ctx,
		struct dom_document *doc, struct dom_document_type **result)
{
	UNUSED(ctx);
	UNUSED(doc);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception dom_document_get_implementation(struct dom_ctx *ctx,
		struct dom_document *doc,
		struct dom_implementation **result)
{
	UNUSED(ctx);
	UNUSED(doc);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception dom_document_get_element(struct dom_ctx *ctx,
		struct dom_document *doc, struct dom_element **result)
{
	UNUSED(ctx);
	UNUSED(doc);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception dom_document_create_element(struct dom_ctx *ctx,
		struct dom_document *doc, struct dom_string *tag_name,
		struct dom_element **result)
{
	UNUSED(ctx);
	UNUSED(doc);
	UNUSED(tag_name);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception dom_document_create_document_fragment(struct dom_ctx *ctx,
		struct dom_document *doc, struct dom_node **result)
{
	UNUSED(ctx);
	UNUSED(doc);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception dom_document_create_text_node(struct dom_ctx *ctx,
		struct dom_document *doc, struct dom_string *data,
		struct dom_text **result)
{
	UNUSED(ctx);
	UNUSED(doc);
	UNUSED(data);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception dom_document_create_cdata_section(struct dom_ctx *ctx,
		struct dom_document *doc, struct dom_string *data,
		struct dom_text **result)
{
	UNUSED(ctx);
	UNUSED(doc);
	UNUSED(data);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception dom_document_create_processing_instruction(struct dom_ctx *ctx,
		struct dom_document *doc, struct dom_string *target,
		struct dom_string *data,
		struct dom_node **result)
{
	UNUSED(ctx);
	UNUSED(doc);
	UNUSED(target);
	UNUSED(data);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception dom_document_create_attribute(struct dom_ctx *ctx,
		struct dom_document *doc, struct dom_string *name,
		struct dom_attr **result)
{
	UNUSED(ctx);
	UNUSED(doc);
	UNUSED(name);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception dom_document_create_entity_reference(struct dom_ctx *ctx,
		struct dom_document *doc, struct dom_string *name,
		struct dom_node **result)
{
	UNUSED(ctx);
	UNUSED(doc);
	UNUSED(name);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception dom_document_get_elements_by_tag_name(struct dom_ctx *ctx,
		struct dom_document *doc, struct dom_string *tagname,
		struct dom_nodelist **result)
{
	UNUSED(ctx);
	UNUSED(doc);
	UNUSED(tagname);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception dom_document_import_node(struct dom_ctx *ctx,
		struct dom_document *doc, struct dom_node *node,
		bool deep, struct dom_node **result)
{
	UNUSED(ctx);
	UNUSED(doc);
	UNUSED(node);
	UNUSED(deep);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception dom_document_create_element_ns(struct dom_ctx *ctx,
		struct dom_document *doc, struct dom_string *namespace,
		struct dom_string *qname, struct dom_element **result)
{
	UNUSED(ctx);
	UNUSED(doc);
	UNUSED(namespace);
	UNUSED(qname);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception dom_document_create_attribute_ns(struct dom_ctx *ctx,
		struct dom_document *doc, struct dom_string *namespace,
		struct dom_string *qname, struct dom_attr **result)
{
	UNUSED(ctx);
	UNUSED(doc);
	UNUSED(namespace);
	UNUSED(qname);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception dom_document_get_elements_by_tag_name_ns(struct dom_ctx *ctx,
		struct dom_document *doc, struct dom_string *namespace,
		struct dom_string *localname, struct dom_nodelist **result)
{
	UNUSED(ctx);
	UNUSED(doc);
	UNUSED(namespace);
	UNUSED(localname);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception dom_document_get_element_by_id(struct dom_ctx *ctx,
		struct dom_document *doc, struct dom_string *id,
		struct dom_element **result)
{
	UNUSED(ctx);
	UNUSED(doc);
	UNUSED(id);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception dom_document_get_input_encoding(struct dom_ctx *ctx,
		struct dom_document *doc, struct dom_string **result)
{
	UNUSED(ctx);
	UNUSED(doc);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception dom_document_get_xml_encoding(struct dom_ctx *ctx,
		struct dom_document *doc, struct dom_string **result)
{
	UNUSED(ctx);
	UNUSED(doc);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception dom_document_get_xml_standalone(struct dom_ctx *ctx,
		struct dom_document *doc, bool *result)
{
	UNUSED(ctx);
	UNUSED(doc);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception dom_document_set_xml_standalone(struct dom_ctx *ctx,
		struct dom_document *doc, bool standalone)
{
	UNUSED(ctx);
	UNUSED(doc);
	UNUSED(standalone);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception dom_document_get_xml_version(struct dom_ctx *ctx,
		struct dom_document *doc, struct dom_string **result)
{
	UNUSED(ctx);
	UNUSED(doc);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception dom_document_set_xml_version(struct dom_ctx *ctx,
		struct dom_document *doc, struct dom_string *version)
{
	UNUSED(ctx);
	UNUSED(doc);
	UNUSED(version);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception dom_document_get_strict_error_checking(struct dom_ctx *ctx,
		struct dom_document *doc, bool *result)
{
	UNUSED(ctx);
	UNUSED(doc);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception dom_document_set_strict_error_checking(struct dom_ctx *ctx,
		struct dom_document *doc, bool strict)
{
	UNUSED(ctx);
	UNUSED(doc);
	UNUSED(strict);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception dom_document_get_uri(struct dom_ctx *ctx,
		struct dom_document *doc, struct dom_string **result)
{
	UNUSED(ctx);
	UNUSED(doc);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception dom_document_set_uri(struct dom_ctx *ctx,
		struct dom_document *doc, struct dom_string *uri)
{
	UNUSED(ctx);
	UNUSED(doc);
	UNUSED(uri);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception dom_document_adopt_node(struct dom_ctx *ctx,
		struct dom_document *doc, struct dom_node *node,
		struct dom_node **result)
{
	UNUSED(ctx);
	UNUSED(doc);
	UNUSED(node);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception dom_document_get_dom_config(struct dom_ctx *ctx,
		struct dom_document *doc, struct dom_configuration **result)
{
	UNUSED(ctx);
	UNUSED(doc);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception dom_document_normalize(struct dom_ctx *ctx,
		struct dom_document *doc)
{
	UNUSED(ctx);
	UNUSED(doc);

	return DOM_NOT_SUPPORTED_ERR;
}

dom_exception dom_document_rename_node(struct dom_ctx *ctx,
		struct dom_document *doc, struct dom_node *node,
		struct dom_string *namespace, struct dom_string *qname,
		struct dom_node **result)
{
	UNUSED(ctx);
	UNUSED(doc);
	UNUSED(node);
	UNUSED(namespace);
	UNUSED(qname);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}
