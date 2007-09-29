/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#include <dom/bootstrap/implpriv.h>

#include "utils/namespace.h"

/**
 * Initialise the dom library
 *
 * \param alloc  Pointer to memory (de)allocation function
 * \param pw     Pointer to client-specific private data
 * \return DOM_NO_ERR on success.
 *
 * This should be invoked by the binding's initialiser and must be
 * the first DOM library method called.
 */
dom_exception dom_initialise(dom_alloc alloc, void *pw)
{
	dom_exception err;

	err = _dom_namespace_initialise(alloc, pw);

	return err;
}

/**
 * Finalise the dom library
 *
 * \return DOM_NO_ERR on success.
 *
 * This should be invoked by the binding's finaliser and must be
 * the last DOM library method called.
 */
dom_exception dom_finalise(void)
{
	dom_exception err;

	err = _dom_namespace_finalise();

	return err;
}

