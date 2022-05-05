/// @brief Support methods to help with unit testing for QASSERT.
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

#ifndef QASSERT_MOCK_SUPPORT_HPP
#define QASSERT_MOCK_SUPPORT_HPP

#include "qassert.h"
#include "CppUTestExt/MockSupport.h"

namespace cms {
namespace test {

static constexpr const char* QASSERT_MOCK_NAME  = "QASSERT";
static constexpr const char* ONASSERT_FUNC_NAME = "Q_onAssert";

inline void MockExpectQAssert()
{
    mock(QASSERT_MOCK_NAME)
      .expectOneCall(ONASSERT_FUNC_NAME)
      .ignoreOtherParameters();
}

inline void MockExpectQAssert(const char* file, int loc)
{
    mock(QASSERT_MOCK_NAME)
      .expectOneCall(ONASSERT_FUNC_NAME)
      .withParameter("file", file)
      .withParameter("loc", loc);
}

}   // namespace test
}   // namespace cms

#endif   // QASSERT_MOCK_SUPPORT_HPP
