/// @file hwLockCtrlService.h
/// @brief A service (active object) for demonstrating cpputest host
///        based unit testing within the QP/C (qpc) Framework.
///        The Hw Lock Ctrl Service demonstration active object provides for
///        higher level hardware lock control behavior. For example, this
///        service will automatically return the hardware lock to its
///        last state after completing a self test request.
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

#ifndef HW_LOCK_CTRL_SERVICE_H
#define HW_LOCK_CTRL_SERVICE_H

#include "qpc.h"
#include "pub_sub_signals.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Direct signals / events that may
 * be sent to the Hw Lock Ctrl Service.
 */
typedef enum {
    HW_LOCK_CTRL_PING_SIG = MAX_PUB_SUB_SIG,
    HW_LOCK_CTRL_DEMONSTRATE_TEST_OF_QASSERT_SIG,
    HW_LOCK_CTRL_MAX_DIRECT_SIG_,
} HwLockCtrlDirectSignals;

/**
 * opaque pointer to the active object, this pointer is
 * only valid after calling HwLockCtrlService_ctor().
 * After calling HwLockCtrlService_dtor(), the pointer
 * will be null.
 */
extern QActive * g_theHwLockCtrlService;

/**
 * Construct the Hw Lock Ctrl Service.
 */
void HwLockCtrlService_ctor();

/**
 * Destroy the Hw Lock Ctrl Service.
 * Typically only needed from a unit
 * testing environment.
 */
void HwLockCtrlService_dtor();

#ifdef __cplusplus
}
#endif

#endif   // HW_LOCK_CTRL_SERVICE_H
