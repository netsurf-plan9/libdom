/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *			http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#include <inttypes.h>
#include <stddef.h>

#include "utils/validate.h"

#include <dom/core/string.h>

#include "utils/character_valid.h"
#include "utils/namespace.h"
#include "utils/utils.h"

/* An combination of various tests */
static bool is_first_char(uint32_t ch);
static bool is_name_char(uint32_t ch);

/* Test whether the character can be the first character of
 * a NCName. */
static bool is_first_char(uint32_t ch)
{
	/* Refer http://www.w3.org/TR/REC-xml/ for detail */
	if (((ch >= 'a') && (ch <= 'z')) ||
		((ch >= 'A') && (ch <= 'Z')) ||
		(ch == '_') || (ch == ':') ||
		((ch >= 0xC0) && (ch <= 0xD6)) ||
		((ch >= 0xD8) && (ch <= 0xF6)) ||
		((ch >= 0xF8) && (ch <= 0x2FF)) ||
		((ch >= 0x370) && (ch <= 0x37D)) ||
		((ch >= 0x37F) && (ch <= 0x1FFF)) ||
		((ch >= 0x200C) && (ch <= 0x200D)) ||
		((ch >= 0x2070) && (ch <= 0x218F)) ||
		((ch >= 0x2C00) && (ch <= 0x2FEF)) ||
		((ch >= 0x3001) && (ch <= 0xD7FF)) ||
		((ch >= 0xF900) && (ch <= 0xFDCF)) ||
		((ch >= 0xFDF0) && (ch <= 0xFFFD)) ||
		((ch >= 0x10000) && (ch <= 0xEFFFF)))
		return true;

	if (is_letter(ch) || ch == (uint32_t) '_' || ch == (uint32_t) ':') {
		return true;
	} 

	return false;
}

/* Test whether the character can be a part of a NCName */
static bool is_name_char(uint32_t ch)
{
	/* Refer http://www.w3.org/TR/REC-xml/ for detail */
	if (((ch >= 'a') && (ch <= 'z')) ||
		((ch >= 'A') && (ch <= 'Z')) ||
		((ch >= '0') && (ch <= '9')) || /* !start */
		(ch == '_') || (ch == ':') ||
		(ch == '-') || (ch == '.') || (ch == 0xB7) || /* !start */
		((ch >= 0xC0) && (ch <= 0xD6)) ||
		((ch >= 0xD8) && (ch <= 0xF6)) ||
		((ch >= 0xF8) && (ch <= 0x2FF)) ||
		((ch >= 0x300) && (ch <= 0x36F)) || /* !start */
		((ch >= 0x370) && (ch <= 0x37D)) ||
		((ch >= 0x37F) && (ch <= 0x1FFF)) ||
		((ch >= 0x200C) && (ch <= 0x200D)) ||
		((ch >= 0x203F) && (ch <= 0x2040)) || /* !start */
		((ch >= 0x2070) && (ch <= 0x218F)) ||
		((ch >= 0x2C00) && (ch <= 0x2FEF)) ||
		((ch >= 0x3001) && (ch <= 0xD7FF)) ||
		((ch >= 0xF900) && (ch <= 0xFDCF)) ||
		((ch >= 0xFDF0) && (ch <= 0xFFFD)) ||
		((ch >= 0x10000) && (ch <= 0xEFFFF)))
		return true;

	if (is_letter(ch) == true)
		return true;
	if (is_digit(ch) == true)
		return true;
	if (is_combining_char(ch) == true)
		return true;
	if (is_extender(ch) == true)
		return true;
	
	if (ch == (uint32_t) '.' || ch == (uint32_t) '-' || 
			ch == (uint32_t) '_' || ch == (uint32_t) ':')
		return true;

	return false;
}

/**
 * Test whether the name is a valid one according XML 1.0 standard.
 * For the standard please refer:
 *
 * http://www.w3.org/TR/2004/REC-xml-20040204/
 *
 * \param name  The name need to be tested
 * \return true if ::name is valid, false otherwise.
 */
bool _dom_validate_name(struct dom_string *name)
{
	uint32_t ch, len, i;
	dom_exception err;

	if (name == NULL)
		return false;

	len = dom_string_length(name);
	if (len == 0)
		return false;

	/* Test the first character of this string */
	err = dom_string_at(name, 0, &ch);
	if (err != DOM_NO_ERR)
		return false;

	if (is_first_char(ch) == false)
		return false;

	/* Test all remain characters in this string */
	for(i = 1; i < len; i++) {
		err = dom_string_at(name, i, &ch);
		if (err != DOM_NO_ERR)
			return false;

		if (is_name_char(ch) != true)
			return false;
	}

	return true;
}

/**
 * Validate whether the string is a legal NCName.
 * Refer http://www.w3.org/TR/REC-xml-names/ for detail.
 *
 * \param str  The name to validate
 * \return true if ::name is valid, false otherwise.
 */
bool _dom_validate_ncname(struct dom_string *name)
{
	uint32_t ch, len, i;
	dom_exception err;

	if (name == NULL)
		return false;

	len = dom_string_length(name);
	if (len == 0)
		return false;

	/* Test the first character of this string */
	err = dom_string_at(name, 0, &ch);
	if (err != DOM_NO_ERR)
		return false;

	if (is_letter(ch) == false && ch != (uint32_t) '_')
		return false;
	
	/* Test all remain characters in this string */
	for(i = 1; i < len; i++) {
		err = dom_string_at(name, i, &ch);
		if (err != DOM_NO_ERR)
			return false;

		if (is_name_char(ch) == false)
			return false;

		if (ch == (uint32_t) ':')
			return false;
	}

	return true;
}

