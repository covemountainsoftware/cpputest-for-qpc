/// @brief Supporting macro for CMS or C++ code that needs to initialize a
///        QEvt data structure.
/// @ingroup
/// @cond
///***************************************************************************
///
/// Copyright (C) 2026 Matthew Eshleman. All rights reserved.
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

#ifndef CMS_QEVT_INITIALIZER_HPP
#define CMS_QEVT_INITIALIZER_HPP

#ifndef __cplusplus
    #error "This header intended for use only within the C++ domain."
#endif

#include <cstddef>
#include "qp.h"

#if QP_VERSION < 810

    //For older QP/C version, just use the macro provided by QP
    #define CMS_QEVT_INITIALIZER(sig) QEVT_INITIALIZER(sig)

#else
    //Newer QP releases use designated initializers,
    //which sadly are not available until C++20.
#if __cplusplus >= 202002L
    //if this project is C++20 based, use QP's macro as is.
    #define CMS_QEVT_INITIALIZER(sig) QEVT_INITIALIZER(sig)
#else
    //for older than C++20, use the implied initializer.
    #define CMS_QEVT_INITIALIZER(sig_) { (QSignal)(sig_), 0x00U, 0xE0U, 0xE0E0E0E0U }
    static_assert(sizeof(QEvt) == 8, "Did the QP library change?");
    static_assert(offsetof(QEvt, filler_)  == 4, "Did the QP library change?");
#endif //c++ version

#endif //QP_VERSION

#endif //CMS_QEVT_INITIALIZER_HPP
