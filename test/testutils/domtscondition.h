/*
 * This file is part of libdom test suite.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#ifndef domtscondition_h_
#define domtscondition_h_

#include <stdbool.h>

inline bool less(int excepted, int actual);
inline bool less_or_equals(int excepted, int actual);
inline bool greater(int excepted, int actual);
inline bool greater_or_equals(int excepted, int actual);

#endif
