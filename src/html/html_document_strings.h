/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2012 Daniel Silverstone <dsilvers@netsurf-browser.org>
 */

/* Note, this file deliberately lacks guards since it's included many times
 * in many places in order to correctly handle the loading of the strings.
 */

#ifndef HTML_DOCUMENT_STRINGS_ACTION
#define HTML_DOCUMENT_STRINGS_INTERNAL_ACTION 1
#define HTML_DOCUMENT_STRINGS_PREFIX      \
	typedef enum {
#define HTML_DOCUMENT_STRINGS_SUFFIX		\
	hds_COUNT				\
	} html_document_memo_string_e;
#define HTML_DOCUMENT_STRINGS_ACTION(tag)	\
	hds_##tag,
#endif

#ifdef HTML_DOCUMENT_STRINGS_PREFIX
HTML_DOCUMENT_STRINGS_PREFIX
#endif

HTML_DOCUMENT_STRINGS_ACTION(id)
HTML_DOCUMENT_STRINGS_ACTION(title)
HTML_DOCUMENT_STRINGS_ACTION(lang)
HTML_DOCUMENT_STRINGS_ACTION(dir)
HTML_DOCUMENT_STRINGS_ACTION(class)

#ifdef HTML_DOCUMENT_STRINGS_SUFFIX
HTML_DOCUMENT_STRINGS_SUFFIX
#endif


#ifdef HTML_DOCUMENT_STRINGS_INTERNAL_ACTION
#undef HTML_DOCUMENT_STRINGS_INTERNAL_ACTION
#undef HTML_DOCUMENT_STRINGS_PREFIX
#undef HTML_DOCUMENT_STRINGS_SUFFIX
#undef HTML_DOCUMENT_STRINGS_ACTION
#endif
