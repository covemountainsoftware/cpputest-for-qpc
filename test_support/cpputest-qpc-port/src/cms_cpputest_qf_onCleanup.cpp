/// @brief Support method for cleanup of a fake cpputest compatible
///        port of the qpc QF framework.
/// @ingroup
/// @cond
///***************************************************************************
///
/// Copyright (C) 2022 Matthew Eshleman. All rights reserved.
///
/// This program is open source software: you can redistribute it and/or
/// modify it under the terms of the GNU General Public License as published
/// by the Free Software Foundation, either version 3 of the License, or
/// (at your option) any later version.
///
/// Alternatively, upon written permission from Matthew Eshleman, this program
/// may be distributed and modified under the terms of a Commercial
/// License. For further details, see the Contact Information below.
///
/// Contact Information:
///   Matthew Eshleman
///   https://covemountainsoftware.com
///   info@covemountainsoftware.com
///***************************************************************************
/// @endcond
#define QP_IMPL //allow access to some QF/QP internals for test purposes
#include "qpc.h"
#include "CppUTest/TestHarness.h"

extern "C" void QF_onCleanup(void)
{
    for (int i = 0; i < QF_maxPool_; ++i) {
        CHECK_TRUE_TEXT(QF_pool_[i].nTot == QF_pool_[i].nFree,
                        "A leak was detected in an internal QF event pool!");
    }
}
