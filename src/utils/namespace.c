/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#include <string.h>

#include <dom/core/string.h>

#include "utils/namespace.h"
#include "utils/utils.h"

/** XML namespace URI */
static struct dom_string *xml;
/** XMLNS namespace URI */
static struct dom_string *xmlns;

/**
 * Initialise the namespace component
 *
 * \param alloc  Pointer to memory (de)allocation function
 * \param pw     Pointer to client-specific private data
 * \return DOM_NO_ERR on success.
 */
dom_exception _dom_namespace_initialise(dom_alloc alloc, void *pw)
{
	dom_exception err;

	err = dom_string_create_from_ptr_no_doc(alloc, pw,
		DOM_STRING_UTF8,
		(const uint8_t *) "http://www.w3.org/XML/1998/namespace", 
		SLEN("http://www.w3.org/XML/1998/namespace"),
		&xml);
	if (err != DOM_NO_ERR) {
		return err;
	}

	err = dom_string_create_from_ptr_no_doc(alloc, pw,
		DOM_STRING_UTF8,
		(const uint8_t *) "http://www.w3.org/2000/xmlns",
		SLEN("http://www.w3.org/2000/xmlns"),
		&xmlns);
	if (err != DOM_NO_ERR) {
		dom_string_unref(xml);
		return err;
	}

	return DOM_NO_ERR;
}

/**
 * Finalise the namespace component
 *
 * \return DOM_NO_ERR on success.
 */
dom_exception _dom_namespace_finalise(void)
{
	dom_string_unref(xmlns);
	dom_string_unref(xml);

	return DOM_NO_ERR;
}

/**
 * Ensure a QName is valid
 *
 * \param qname      The qname to validate
 * \param namespace  The namespace URI associated with the QName, or NULL
 * \return DOM_NO_ERR                if valid,
 *         DOM_INVALID_CHARACTER_ERR if ::qname contains an invalid character,
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
 *                                   "xmlns".
 */
dom_exception _dom_namespace_validate_qname(struct dom_string *qname,
		struct dom_string *namespace)
{
	const uint8_t *qname_data, *c;
	size_t qname_len;

	dom_string_get_data(qname, &qname_data, &qname_len);

	/** \todo search qname for invalid characters */
	/** \todo ensure qname is not malformed */

	/* Find colon */
	/** \todo assumes ASCII-compatible encoding */
	for (c = qname_data; c != qname_data + qname_len; c++) {
		if (*c == (const uint8_t) ':') {
			break;
		}
	}

	if (c == qname_data + qname_len) {
		/* No prefix */
		/* If namespace URI is for xmlns, ensure qname == "xmlns" */
		if (namespace != NULL && 
				dom_string_cmp(namespace, xmlns) == 0 &&
				(qname_len != SLEN("xmlns") || 
				 strncmp((const char *) qname_data, "xmlns", 
					 SLEN("xmlns")) != 0)) {
			return DOM_NAMESPACE_ERR;
		}
	} else {
		/* Prefix */
		/* Ensure there is a namespace URI */
		if (namespace == NULL) {
			return DOM_NAMESPACE_ERR;
		}

		/* Test for invalid XML namespace */
		if (c - qname_data == SLEN("xml") &&
				strncmp((const char *) qname_data, "xml", 
						SLEN("xml")) == 0 &&
				dom_string_cmp(namespace, xml) != 0) {
			return DOM_NAMESPACE_ERR;
		}

		/* Test for invalid xmlns namespace */
		if (c - qname_data == SLEN("xmlns") &&
			strncmp((const char *) qname_data, "xmlns", 
					SLEN("xmlns")) == 0 &&
				dom_string_cmp(namespace, xmlns) != 0) {
			return DOM_NAMESPACE_ERR;
		}

		/* Test for presence of xmlns namespace with non xmlns prefix */
		if (dom_string_cmp(namespace, xmlns) == 0 &&
				(c - qname_data != SLEN("xmlns") ||
				 strncmp((const char *) qname_data, "xmlns", 
						 SLEN("xmlns")) != 0)) {
			return DOM_NAMESPACE_ERR;
		}
	}

	return DOM_NO_ERR;
}

/**
 * Split a QName into a namespace prefix and localname string
 *
 * \param qname      The qname to split
 * \param doc        The document context to create the prefix/localname in
 * \param prefix     Pointer to location to receive prefix
 * \param localname  Pointer to location to receive localname
 * \return DOM_NO_ERR on success.
 *
 * If there is no prefix present in ::qname, then ::prefix will be NULL.
 *
 * ::prefix and ::localname will be referenced. The caller should unreference
 * them once finished.
 */
dom_exception _dom_namespace_split_qname(struct dom_string *qname,
		struct dom_document *doc, struct dom_string **prefix, 
		struct dom_string **localname)
{
	const uint8_t *qname_data, *c, *local_data;
	size_t qname_len;
	size_t local_len;
	size_t prefix_len;
	struct dom_string *p = NULL;
	struct dom_string *l;
	dom_exception err;

	dom_string_get_data(qname, &qname_data, &qname_len);

	/* Find colon, if any */
	/** \todo assumes ASCII-compatible encoding */
	for (c = qname_data; c != qname_data + qname_len; c++) {
		if (*c == (const uint8_t) ':')
			break;
	}

	if (c == qname_data + qname_len) {
		/* None found => no prefix */
		local_data = qname_data;
		local_len = qname_len;
		prefix_len = 0;
	} else {
		/* Found one => prefix */
		local_data = ++c;
		local_len = qname_len - (c - qname_data);
		prefix_len = (c - qname_data - 1 /* ':' */);
	}

	/* Create prefix, if one exists */
	if (prefix_len > 0) {
		err = dom_string_create_from_ptr(doc, qname_data, 
				prefix_len, &p);
		if (err != DOM_NO_ERR) {
			return err;
		}
	}

	/* Create localname */
	err = dom_string_create_from_ptr(doc, local_data, local_len, &l);
	if (err != DOM_NO_ERR) {
		if (p != NULL) {
			dom_string_unref(p);
		}
		return err;
	}

	*prefix = p;
	*localname = l;

	return DOM_NO_ERR;
}

