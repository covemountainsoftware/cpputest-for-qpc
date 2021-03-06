/// @brief The HwLockCtrl Self Test Result Enum.
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

#ifndef HWLOCKCTRLSELFTESTRESULTENUM_H
#define HWLOCKCTRLSELFTESTRESULTENUM_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HW_LOCK_CTRL_SELF_TEST_RESULT_PASS,
    HW_LOCK_CTRL_SELF_TEST_RESULT_FAIL,
} SelfTestResult;

#ifdef __cplusplus
}
#endif

#endif   // HWLOCKCTRLSELFTESTRESULTENUM_H
