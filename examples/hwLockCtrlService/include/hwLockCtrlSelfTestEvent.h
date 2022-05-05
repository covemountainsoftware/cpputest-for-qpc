/// @brief The HwLockCtrl Self Test Event struct.
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

#ifndef HWLOCKCTRL_SELF_TEST_EVENT_H
#define HWLOCKCTRL_SELF_TEST_EVENT_H

#include "qpc.h"
#include "hwLockCtrlSelfTestResultEnum.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Event type holding the results of a
/// HwLockCtrl Service Self Test.
typedef struct {
    QEvt super; /* inherit QEvt, via QP/C Framework C style */

    SelfTestResult result;
} SelfTestEvent;

#ifdef __cplusplus
}
#endif

#endif   // HWLOCKCTRL_SELF_TEST_EVENT_H
