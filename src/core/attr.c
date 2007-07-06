/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#include <dom/core/attr.h>

#include "core/node.h"
#include "utils/utils.h"

struct dom_element;
struct dom_typeinfo;

/**
 * DOM node attribute
 */
struct dom_attr {
	struct dom_node base;		/**< Base node */

	bool specified;			/**< Whether attribute was specified
					 * or defaulted */

	struct dom_element *owner;	/**< Owning element */

	struct dom_typeinfo *schema_type_info;	/**< Type information */

	bool is_id;			/**< Attribute is of type ID */
};

/**
 * Retrieve an attribute's name
 *
 * \param attr    Attribute to retrieve name from
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception dom_attr_get_name(struct dom_attr *attr,
		struct dom_string **result)
{
	UNUSED(attr);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Determine if attribute was specified or defaulted
 *
 * \param attr    Attribute to inspect
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 */
dom_exception dom_attr_get_specified(struct dom_attr *attr, bool *result)
{

	*result = attr->specified;

	return DOM_NO_ERR;
}

/**
 * Retrieve an attribute's value
 *
 * \param attr    Attribute to retrieve value from
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned string will have its reference count increased. It is
 * the responsibility of the caller to unref the string once it has
 * finished with it.
 */
dom_exception dom_attr_get_value(struct dom_attr *attr,
		struct dom_string **result)
{
	UNUSED(attr);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Set an attribute's value
 *
 * \param attr   Attribute to retrieve value from
 * \param value  New value for attribute
 * \return DOM_NO_ERR                      on success,
 *         DOM_NO_MODIFICATION_ALLOWED_ERR if attribute is readonly.
 */
dom_exception dom_attr_set_value(struct dom_attr *attr,
		struct dom_string *value)
{
	UNUSED(attr);
	UNUSED(value);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Retrieve the owning element of an attribute
 *
 * \param attr    The attribute to extract owning element from
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned node will have its reference count increased. The caller
 * should unref it once it has finished with it.
 */
dom_exception dom_attr_get_owner(struct dom_attr *attr,
		struct dom_element **result)
{
	/* If there is an owning element, increase its reference count */
	if (attr->owner != NULL)
		dom_node_ref((struct dom_node *) attr->owner);

	*result = attr->owner;

	return DOM_NO_ERR;
}

/**
 * Retrieve an attribute's type information
 *
 * \param attr    The attribute to extract type information from
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 *
 * The returned typeinfo will have its reference count increased. The caller
 * should unref it once it has finished with it.
 */
dom_exception dom_attr_get_typeinfo(struct dom_attr *attr,
		struct dom_typeinfo **result)
{
	UNUSED(attr);
	UNUSED(result);

	return DOM_NOT_SUPPORTED_ERR;
}

/**
 * Determine if an attribute if of type ID
 *
 * \param attr    The attribute to inspect
 * \param result  Pointer to location to receive result
 * \return DOM_NO_ERR.
 */
dom_exception dom_attr_is_id(struct dom_attr *attr, bool *result)
{
	*result = attr->is_id;

	return DOM_NO_ERR;
}
