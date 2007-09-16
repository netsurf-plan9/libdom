/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2007 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef xml_functypes_h_
#define xml_functypes_h_

/**
 * Type of XML parser allocation function
 */
typedef void *(*xml_alloc)(void *ptr, size_t len, void *pw);

/**
 * Severity levels for xml_msg function, based on syslog(3)
 */
enum {
	XML_MSG_DEBUG,
	XML_MSG_INFO,
	XML_MSG_NOTICE,
	XML_MSG_WARNING,
	XML_MSG_ERROR,
	XML_MSG_CRITICAL,
	XML_MSG_ALERT,
	XML_MSG_EMERGENCY
};

/**
 * Type of XML parser message function
 */
typedef void (*xml_msg)(uint32_t severity, void *ctx, const char *msg, ...);

#endif

