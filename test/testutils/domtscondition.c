/*
 * This file is part of libdom test suite.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#include <stdbool.h>

/**
 * Just simple functions which meet the needs of DOMTS conditions
 */

bool less(int excepted, int actual)
{
	return actual < excepted;
}

bool less_or_equals(int excepted, int actual)
{
	return actual <= excepted;
}

bool greater(int excepted, int actual)
{
	return actual > excepted;
}

bool greater_or_equals(int excepted, int actual)
{
	return actual >= excepted;
}
