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

#ifndef CMS_TEST_QASSERT_MOCK_SUPPORT_HPP
#define CMS_TEST_QASSERT_MOCK_SUPPORT_HPP

#include "qp_port.h"
#include "qsafe.h"
#include "CppUTestExt/MockSupport.h"

namespace cms {
namespace test {

static constexpr const char* QASSERT_MOCK_NAME  = "QASSERT";
static constexpr const char* ONERROR_FUNC_NAME  = "Q_onError";

inline void MockExpectQAssert()
{
    mock(QASSERT_MOCK_NAME)
      .expectOneCall(ONERROR_FUNC_NAME)
      .ignoreOtherParameters();
}

inline void MockExpectQAssert(const char* module, int id)
{
    mock(QASSERT_MOCK_NAME)
      .expectOneCall(ONERROR_FUNC_NAME)
      .withParameter("module", module)
      .withParameter("id", id);
}

}   // namespace test
}   // namespace cms

#endif   // CMS_TEST_QASSERT_MOCK_SUPPORT_HPP
