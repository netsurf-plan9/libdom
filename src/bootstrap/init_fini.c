/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#include <stdbool.h>

#include <dom/bootstrap/init_fini.h>

#include "utils/namespace.h"

static bool __initialised;

/**
 * Initialise the dom library
 *
 * \param alloc  Pointer to memory (de)allocation function
 * \param pw     Pointer to client-specific private data
 * \return DOM_NO_ERR on success.
 *
 * This must be the first DOM library method called.
 */
dom_exception dom_initialise(dom_alloc alloc, void *pw)
{
	dom_exception err;

	/* Ensure we only initialise once */
	if (__initialised) {
		return DOM_NO_ERR;
	}

	err = _dom_namespace_initialise(alloc, pw);

	if (err == DOM_NO_ERR) {
		__initialised = true;
	}

	return err;
}

/**
 * Finalise the dom library
 *
 * \return DOM_NO_ERR on success.
 *
 * This must be the last DOM library method called.
 */
dom_exception dom_finalise(void)
{
	dom_exception err;

	/* Ensure we only finalise once */
	if (__initialised == false) {
		return DOM_NO_ERR;
	}

	err = _dom_namespace_finalise();

	__initialised = false;

	return err;
}

