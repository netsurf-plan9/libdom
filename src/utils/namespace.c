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

/** XML prefix */
static struct dom_string *xml;
/** XML namespace URI */
static struct dom_string *xml_ns;
/** XMLNS prefix */
static struct dom_string *xmlns;
/** XMLNS namespace URI */
static struct dom_string *xmlns_ns;

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
		DOM_STRING_UTF8, (const uint8_t *) "xml", SLEN("xml"), &xml);
	if (err != DOM_NO_ERR) {
		return err;
	}

	err = dom_string_create_from_ptr_no_doc(alloc, pw,
		DOM_STRING_UTF8,
		(const uint8_t *) "http://www.w3.org/XML/1998/namespace", 
		SLEN("http://www.w3.org/XML/1998/namespace"),
		&xml_ns);
	if (err != DOM_NO_ERR) {
		dom_string_unref(xml);
		return err;
	}

	err = dom_string_create_from_ptr_no_doc(alloc, pw,
		DOM_STRING_UTF8, 
		(const uint8_t *) "xmlns", SLEN("xmlns"), &xmlns);
	if (err != DOM_NO_ERR) {
		dom_string_unref(xml_ns);
		dom_string_unref(xml);
		return err;
	}

	err = dom_string_create_from_ptr_no_doc(alloc, pw,
		DOM_STRING_UTF8,
		(const uint8_t *) "http://www.w3.org/2000/xmlns",
		SLEN("http://www.w3.org/2000/xmlns"),
		&xmlns_ns);
	if (err != DOM_NO_ERR) {
		dom_string_unref(xmlns);
		dom_string_unref(xml_ns);
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
	dom_string_unref(xmlns_ns);
	dom_string_unref(xmlns);
	dom_string_unref(xml_ns);
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
	uint32_t colon;

	/** \todo search qname for invalid characters */
	/** \todo ensure qname is not malformed */

	/* Find colon */
	colon = dom_string_index(qname, ':');

	if (colon == (uint32_t) -1) {
		/* No prefix */
		/* If namespace URI is for xmlns, ensure qname == "xmlns" */
		if (namespace != NULL && 
				dom_string_cmp(namespace, xmlns_ns) == 0 &&
				dom_string_cmp(qname, xmlns) != 0) {
			return DOM_NAMESPACE_ERR;
		}
	} else {
		/* Prefix */
		struct dom_string *prefix;
		dom_exception err;

		/* Ensure there is a namespace URI */
		if (namespace == NULL) {
			return DOM_NAMESPACE_ERR;
		}

		err = dom_string_substr(qname, 0, colon - 1, &prefix);
		if (err != DOM_NO_ERR) {
			return err;
		}

		/* Test for invalid XML namespace */
		if (dom_string_cmp(prefix, xml) == 0 &&
				dom_string_cmp(namespace, xml_ns) != 0) {
			dom_string_unref(prefix);
			return DOM_NAMESPACE_ERR;
		}

		/* Test for invalid xmlns namespace */
		if (dom_string_cmp(prefix, xmlns) == 0 &&
				dom_string_cmp(namespace, xmlns_ns) != 0) {
			dom_string_unref(prefix);
			return DOM_NAMESPACE_ERR;
		}

		/* Test for presence of xmlns namespace with non xmlns prefix */
		if (dom_string_cmp(namespace, xmlns_ns) == 0 &&
				dom_string_cmp(prefix, xmlns) != 0) {
			dom_string_unref(prefix);
			return DOM_NAMESPACE_ERR;
		}

		dom_string_unref(prefix);
	}

	return DOM_NO_ERR;
}

/**
 * Split a QName into a namespace prefix and localname string
 *
 * \param qname      The qname to split
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
		struct dom_string **prefix, struct dom_string **localname)
{
	uint32_t colon;
	dom_exception err;

	/* Find colon, if any */
	colon = dom_string_index(qname, ':');

	if (colon == (uint32_t) -1) {
		/* None found => no prefix */
		*prefix = NULL;
		err = dom_string_dup(qname, localname);
		if (err != DOM_NO_ERR) {
			return err;
		}
	} else {
		/* Found one => prefix */
		err = dom_string_substr(qname, 0, colon - 1, prefix);
		if (err != DOM_NO_ERR) {
			return err;
		}

		err = dom_string_substr(qname, colon + 1,
				dom_string_length(qname), localname);
		if (err != DOM_NO_ERR) {
			dom_string_unref(*prefix);
			*prefix = NULL;
			return err;
		}
	}

	return DOM_NO_ERR;
}

