/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#include <assert.h>

#include "events/dispatch.h"
#include "events/mutation_event.h"

#include "utils/utils.h"

/**
 * Dispatch a DOMNodeInserted/DOMNodeRemoved event
 *
 * \param doc      The document object
 * \param et       The EventTarget object
 * \param type     "DOMNodeInserted" or "DOMNodeRemoved"
 * \param related  The parent of the removed/inserted node
 * \param success  Whether this event's default action get called
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception __dom_dispatch_node_change_event(dom_document *doc,
		dom_event_target *et, dom_event_target *related, 
		dom_mutation_type change, bool *success)
{
	struct dom_mutation_event *evt;
	dom_string *type = NULL;
	dom_exception err;

	err = _dom_mutation_event_create(doc, &evt);
	if (err != DOM_NO_ERR)
		return err;
	
	if (change == DOM_MUTATION_ADDITION) {
		err = dom_string_create((const uint8_t *) "DOMNodeInserted",
				SLEN("DOMNodeInserted"), &type);
		if (err != DOM_NO_ERR)
			goto cleanup;
	} else if (change == DOM_MUTATION_REMOVAL) {
		err = dom_string_create((const uint8_t *) "DOMNodeRemoval",
				SLEN("DOMNodeRemoved"), &type);
		if (err != DOM_NO_ERR)
			goto cleanup;
	} else {
		assert("Should never be here" == NULL);
	}

	/* Initialise the event with corresponding parameters */
	err = dom_mutation_event_init(evt, type, true, false, 
			related, NULL, NULL, NULL, change);
	dom_string_unref(type);
	if (err != DOM_NO_ERR) {
		goto cleanup;
	}

	err = dom_event_target_dispatch_event(et, evt, success);
	if (err != DOM_NO_ERR)
		goto cleanup;
	
cleanup:
	_dom_mutation_event_destroy(evt);

	return err;
}

/**
 * Dispatch a DOMNodeInsertedIntoDocument/DOMNodeRemovedFromDocument event
 *
 * \param doc      The document object
 * \param et       The EventTarget object
 * \param type     "DOMNodeInserted" or "DOMNodeRemoved"
 * \param success  Whether this event's default action get called
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception __dom_dispatch_node_change_document_event(dom_document *doc,
		dom_event_target *et, dom_mutation_type change, bool *success)
{
	struct dom_mutation_event *evt;
	dom_string *type = NULL;
	dom_exception err;

	err = _dom_mutation_event_create(doc, &evt);
	if (err != DOM_NO_ERR)
		return err;

	if (change == DOM_MUTATION_ADDITION) {
		err = dom_string_create(
				(const uint8_t *) 
					"DOMNodeInsertedIntoDocument", 
				SLEN("DOMNodeInsertedIntoDocument"), &type);
		if (err != DOM_NO_ERR)
			goto cleanup;
	} else if (change == DOM_MUTATION_REMOVAL) {
		err = dom_string_create(
				(const uint8_t *) "DOMNodeRemovedFromDocument", 
				SLEN("DOMNodeRemovedFromDocument"), &type);
		if (err != DOM_NO_ERR)
			goto cleanup;
	} else {
		assert("Should never be here" == NULL);
	}

	/* Initialise the event with corresponding parameters */
	err = dom_mutation_event_init(evt, type, true, false, NULL,
			NULL, NULL, NULL, change);
	dom_string_unref(type);
	if (err != DOM_NO_ERR)
		goto cleanup;

	err = dom_event_target_dispatch_event(et, evt, success);
	if (err != DOM_NO_ERR)
		goto cleanup;
	
cleanup:
	_dom_mutation_event_destroy(evt);

	return err;
}

/**
 * Dispatch a DOMAttrModified event
 *
 * \param doc        The Document object
 * \param et         The EventTarget
 * \param prev       The previous value before change
 * \param new        The new value after change
 * \param related    The related EventTarget
 * \param attr_name  The Attribute name
 * \param change     How this attribute change
 * \param success    Whether this event's default handler get called
 * \return DOM_NO_ERR on success, appropirate dom_exception on failure.
 */
dom_exception __dom_dispatch_attr_modified_event(dom_document *doc,
		dom_event_target *et, dom_string *prev, dom_string *new,
		dom_event_target *related, dom_string *attr_name, 
		dom_mutation_type change, bool *success)
{
	struct dom_mutation_event *evt;
	dom_string *type = NULL;
	dom_exception err;

	err = _dom_mutation_event_create(doc, &evt);
	if (err != DOM_NO_ERR)
		return err;
	
	err = dom_string_create((const uint8_t *) "DOMAttrModified",
			SLEN("DOMAttrModified"), &type);
	if (err != DOM_NO_ERR)
		goto cleanup;

	/* Initialise the event with corresponding parameters */
	err = dom_mutation_event_init(evt, type, true, false, related, 
			prev, new, attr_name, change);
	dom_string_unref(type);
	if (err != DOM_NO_ERR) {
		goto cleanup;
	}

	err = dom_event_target_dispatch_event(et, evt, success);

cleanup:
	_dom_mutation_event_destroy(evt);

	return err;
}

/**
 * Dispatch a DOMCharacterDataModified event
 *
 * \param et       The EventTarget object
 * \param prev     The preValue of the DOMCharacterData
 * \param new      The newValue of the DOMCharacterData
 * \param success  Whether this event's default handler get called
 * \return DOM_NO_ERR on success, appropirate dom_exception on failure.
 *
 * TODO:
 * The character_data object may be a part of a Attr node, if so, another 
 * DOMAttrModified event should be dispatched, too. But for now, we did not
 * support any XML feature, so just leave it as this.
 */
dom_exception __dom_dispatch_characterdata_modified_event(
		dom_document *doc, dom_event_target *et,
		dom_string *prev, dom_string *new, bool *success)
{
	struct dom_mutation_event *evt;
	dom_string *type = NULL;
	dom_exception err;

	err = _dom_mutation_event_create(doc, &evt);
	if (err != DOM_NO_ERR)
		return err;
	
	err = dom_string_create((const uint8_t *) "DOMCharacterDataModified",
			SLEN("DOMCharacterDataModified"), &type);
	if (err != DOM_NO_ERR)
		goto cleanup;

	err = dom_mutation_event_init(evt, type, true, false, et, prev, 
			new, NULL, DOM_MUTATION_MODIFICATION);
	dom_string_unref(type);
	if (err != DOM_NO_ERR) {
		goto cleanup;
	}

	err = dom_event_target_dispatch_event(et, evt, success);

cleanup:
	_dom_mutation_event_destroy(evt);

	return err;
}

/**
 * Dispatch a DOMSubtreeModified event
 *
 * \param doc      The Document
 * \param et       The EventTarget object
 * \param success  Whether this event's default handler get called
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception __dom_dispatch_subtree_modified_event(dom_document *doc,
		dom_event_target *et, bool *success)
{
	struct dom_mutation_event *evt;
	dom_string *type = NULL;
	dom_exception err;

	err = _dom_mutation_event_create(doc, &evt);
	if (err != DOM_NO_ERR)
		return err;
	
	err = dom_string_create((const uint8_t *) "DOMSubtreeModified",
			SLEN("DOMSubtreeModified"), &type);
	if (err != DOM_NO_ERR)
		goto cleanup;

	err = dom_mutation_event_init(evt, type, true, false, et, NULL, 
			NULL, NULL, DOM_MUTATION_MODIFICATION);
	dom_string_unref(type);
	if (err != DOM_NO_ERR) {
		goto cleanup;
	}

	err = dom_event_target_dispatch_event(et, evt, success);

cleanup:
	_dom_mutation_event_destroy(evt);

	return err;
}

/**
 * Dispatch a generic event
 *
 * \param doc         The Document
 * \param et          The EventTarget object
 * \param name        The name of the event
 * \param len         The length of the name string
 * \param bubble      Whether this event bubbles
 * \param cancelable  Whether this event can be cancelable
 * \param success     Whether this event's default handler get called
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_dispatch_generic_event(dom_document *doc,
		dom_event_target *et, const uint8_t *name, size_t len,
		bool bubble, bool cancelable, bool *success)
{
	struct dom_event *evt;
	dom_string *type = NULL;
	dom_exception err;

	err = _dom_event_create(doc, &evt);
	if (err != DOM_NO_ERR)
		return err;
	
	err = dom_string_create(name, len, &type);
	if (err != DOM_NO_ERR)
		goto cleanup;

	err = dom_event_init(evt, type, bubble, cancelable);
	dom_string_unref(type);
	if (err != DOM_NO_ERR) {
		goto cleanup;
	}

	err = dom_event_target_dispatch_event(et, evt, success);

cleanup:
	_dom_event_destroy(evt);

	return err;
}

