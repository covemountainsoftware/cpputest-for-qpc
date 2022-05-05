/// @brief  Tests for the Hw Lock Ctrl Service, demonstrating various unit
///         testing capabilities of the 'fake' cms::test::qf_ctrl environment,
///         using cpputest and QP/C (qpc) framework.
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

#include <array>
#include <chrono>
#include "qpc.h"
#include "hwLockCtrl.h"
#include "pub_sub_signals.h"
#include "hwLockCtrlService.h"
#include "cms_cpputest_qf_ctrl.hpp"
#include "cmsTestPublishedEventRecorder.hpp"
#include "hwLockCtrlSelfTestEvent.h"
#include "bspTicks.h"
#include "qassertMockSupport.hpp"
#include "pingPongEvents.h"
#include "cmsDummyActiveObject.hpp"

// the cpputest headers must always be last
#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

Q_DEFINE_THIS_FILE

static constexpr const char* HW_LOCK_CTRL_MOCK = "HwLockCtrl";

static std::array<QEvt const*, 10> testQueueStorage;

/**
 * @brief These tests demonstrate the following key points:
 *         1) Does NOT test any thread that may be associated with the active
 *            object, rather, the associated cms:test::qf_ctrl environment is
 *            thread free, and "faked" to required the test to drive
 *            processing time.
 *         2) Tests the internal behavior of the active object without
 *            knowledge of the internal state machine. Rather, only
 *            by observing the behavior and associated output/results.
 *         3) Follow software engineering best practices, such
 *            as adhering to the DRY principle.
 *         4) Shows how to test if an event was published to the QP framework,
 *            using a published event recorder available from
 *            cms::test::qf_ctrl.
 *         5) Shows how to test if a published event has a custom type and
 *            payload.
 *         6) Shows how to test behavior driven by QActive timers (i.e. how
 *            to test the forward movement of time and expected behavior being
 *            tested.)
 */
TEST_GROUP(HwLockCtrlServiceTests)
{
    bool skipUnderTestDestructor = false;
    QActive* mUnderTest         = nullptr;
    cms::test::PublishedEventRecorder* mRecorder = nullptr;

    void setup() final
    {
        using namespace cms::test;
        skipUnderTestDestructor = false;
        qf_ctrl::Setup(MAX_PUB_SUB_SIG, BSP_TICKS_PER_SECOND);
        mRecorder = cms::test::PublishedEventRecorder::CreatePublishedEventRecorder(
          qf_ctrl::RECORDER_PRIORITY, Q_USER_SIG, MAX_PUB_SUB_SIG);

        //create our unit under test. Since it is a C style unit,
        //we must call its init method (ctor, following QP/C style)
        CHECK_TRUE(g_theHwLockCtrlService == nullptr);
        HwLockCtrlService_ctor();
        mUnderTest = g_theHwLockCtrlService;
        CHECK_TRUE(mUnderTest != nullptr);
    }

    void teardown() final
    {
        using namespace cms::test;

        if (skipUnderTestDestructor)
        {
            g_theHwLockCtrlService = nullptr;
        }
        else
        {
            HwLockCtrlService_dtor();
            //The dtor for this C version
            // needs processing time.
            qf_ctrl::ProcessEvents();
        }

        mUnderTest = nullptr;
        mock().clear();
        qf_ctrl::Teardown();
        delete mRecorder;
    }

    void startServiceToLocked()
    {
        using namespace cms::test;

        // setup mock to ensure the service under test calls the driver
        // API as expected.
        mock(HW_LOCK_CTRL_MOCK).expectOneCall("Init");
        mock(HW_LOCK_CTRL_MOCK).expectOneCall("Lock");

        // start the active object under test.
        QACTIVE_START(mUnderTest, qf_ctrl::UNIT_UNDER_TEST_PRIORITY,
                          testQueueStorage.data(), testQueueStorage.size(),
                          nullptr, 0, nullptr);

        // give the system some processing time to handle
        // any internal or queued events
        giveProcessingTime();

        // check that the driver/mock interactions were all as expected.
        mock().checkExpectations();

        // check that the event recorder captured the expected published event
        CHECK_TRUE(
          mRecorder->isSignalRecorded(HW_LOCK_CTRL_SERVICE_IS_LOCKED_SIG));
    }

    void startServiceToUnlocked()
    {
        startServiceToLocked();
        testUnlock();
    }

    void testUnlock()
    {
        using namespace cms::test;

        mock(HW_LOCK_CTRL_MOCK).expectOneCall("Unlock");
        qf_ctrl::PublishAndProcess(HW_LOCK_CTRL_SERVICE_REQUEST_UNLOCKED_SIG,
                                   mRecorder);
        mock().checkExpectations();
        CHECK_TRUE(
          mRecorder->isSignalRecorded(HW_LOCK_CTRL_SERVICE_IS_UNLOCKED_SIG));
    }

    void giveProcessingTime()
    {
        using namespace cms::test;
        qf_ctrl::ProcessEvents();
    }
};

TEST(HwLockCtrlServiceTests, given_ctor_called_twice_the_second_call_will_assert)
{
    using namespace cms::test;

    //first ctor call is in the setup.
    //lets call again, it should assert.
    MockExpectQAssert();
    HwLockCtrlService_ctor();
    mock().checkExpectations();
}

TEST(HwLockCtrlServiceTests, given_init_when_created_then_does_not_crash)
{
    // setup() is automatically called by cpputest, which creates our unit under
    // test fully representing this trivial starting test.
}

TEST(HwLockCtrlServiceTests,
     given_startup_when_started_then_service_ensures_the_lock_is_locked)
{
    // When originally developed, this test contained all the code
    // in the below helper method. Since this "setup" was needed by
    // other tests, it was extracted into a helper method.
    // This pattern of developing tests, discovering the need for common
    // setup helper methods, takes place throughout coding of these unit tests,
    // and represents adhering to the DRY principle, even in our unit testing
    // code.
    startServiceToLocked();
}

TEST(HwLockCtrlServiceTests,
     given_locked_when_another_lock_request_then_service_is_silent)
{
    using namespace cms::test;

    startServiceToLocked();
    qf_ctrl::PublishAndProcess(HW_LOCK_CTRL_SERVICE_REQUEST_LOCKED_SIG,
                               mRecorder);
    CHECK_TRUE(mRecorder->isEmpty());
    mock().checkExpectations();
}

TEST(HwLockCtrlServiceTests,
     given_locked_when_unlock_request_then_service_unlocks_the_driver)
{
    startServiceToLocked();
    testUnlock();
}

TEST(HwLockCtrlServiceTests,
     given_unlocked_when_another_unlock_request_then_service_is_silent)
{
    using namespace cms::test;

    startServiceToUnlocked();
    qf_ctrl::PublishAndProcess(HW_LOCK_CTRL_SERVICE_REQUEST_UNLOCKED_SIG,
                               mRecorder);
    CHECK_TRUE(mRecorder->isEmpty());
    mock().checkExpectations();
}

TEST(HwLockCtrlServiceTests,
     given_unlocked_a_lock_request_will_return_to_locked)
{
    using namespace cms::test;

    startServiceToUnlocked();
    mock(HW_LOCK_CTRL_MOCK).expectOneCall("Lock");
    qf_ctrl::PublishAndProcess(HW_LOCK_CTRL_SERVICE_REQUEST_LOCKED_SIG,
                               mRecorder);
    CHECK_TRUE(mRecorder->isSignalRecorded(HW_LOCK_CTRL_SERVICE_IS_LOCKED_SIG));
    mock().checkExpectations();
}

TEST(HwLockCtrlServiceTests,
     given_locked_when_selftest_requested_then_service_performs_selftest_publishes_results_and_returns_to_locked)
{
    using namespace cms::test;

    startServiceToLocked();

    auto passed = HW_LOCK_CTRL_SELF_TEST_PASSED;
    mock(HW_LOCK_CTRL_MOCK)
      .expectOneCall("SelfTest")
      .withOutputParameterReturning("outResult", &passed, sizeof(passed));
    mock(HW_LOCK_CTRL_MOCK).expectOneCall("Lock");
    qf_ctrl::PublishAndProcess(HW_LOCK_CTRL_SERVICE_REQUEST_SELF_TEST_SIG,
                               mRecorder);
    mock().checkExpectations();
    auto event = mRecorder->getRecordedEvent();
    CHECK_TRUE(event != nullptr);
    CHECK_EQUAL(HW_LOCK_CTRL_SERVICE_SELF_TEST_RESULTS_SIG, event->sig);
    CHECK_TRUE(HW_LOCK_CTRL_SELF_TEST_RESULT_PASS ==
               reinterpret_cast<const SelfTestEvent*>(event.get())->result);
    CHECK_TRUE(mRecorder->isSignalRecorded(HW_LOCK_CTRL_SERVICE_IS_LOCKED_SIG));
}

TEST(HwLockCtrlServiceTests,
     given_unlocked_when_selftest_request_then_service_performs_selftest_emits_results_and_returns_to_unlocked)
{
    using namespace cms::test;

    startServiceToUnlocked();

    auto passed = HW_LOCK_CTRL_SELF_TEST_PASSED;
    mock(HW_LOCK_CTRL_MOCK)
      .expectOneCall("SelfTest")
      .withOutputParameterReturning("outResult", &passed, sizeof(passed));
    mock(HW_LOCK_CTRL_MOCK).expectOneCall("Unlock");
    qf_ctrl::PublishAndProcess(HW_LOCK_CTRL_SERVICE_REQUEST_SELF_TEST_SIG,
                               mRecorder);
    mock().checkExpectations();
    auto event = mRecorder->getRecordedEvent();
    CHECK_TRUE(event != nullptr);
    CHECK_EQUAL(HW_LOCK_CTRL_SERVICE_SELF_TEST_RESULTS_SIG, event->sig);
    CHECK_TRUE(HW_LOCK_CTRL_SELF_TEST_RESULT_PASS ==
               reinterpret_cast<const SelfTestEvent*>(event.get())->result);
    CHECK_TRUE(
      mRecorder->isSignalRecorded(HW_LOCK_CTRL_SERVICE_IS_UNLOCKED_SIG));
}

TEST(HwLockCtrlServiceTests,
     given_locked_when_selftest_request_which_fails_then_service_still_returns_to_locked)
{
    using namespace cms::test;

    startServiceToLocked();

    auto passed = HW_LOCK_CTRL_SELF_TEST_FAILED_POWER;
    mock(HW_LOCK_CTRL_MOCK)
      .expectOneCall("SelfTest")
      .withOutputParameterReturning("outResult", &passed, sizeof(passed));
    mock(HW_LOCK_CTRL_MOCK).expectOneCall("Lock");
    qf_ctrl::PublishAndProcess(HW_LOCK_CTRL_SERVICE_REQUEST_SELF_TEST_SIG,
                               mRecorder);
    mock().checkExpectations();
    auto event = mRecorder->getRecordedEvent();
    CHECK_TRUE(event != nullptr);
    CHECK_EQUAL(HW_LOCK_CTRL_SERVICE_SELF_TEST_RESULTS_SIG, event->sig);
    CHECK_TRUE(HW_LOCK_CTRL_SELF_TEST_RESULT_FAIL ==
               reinterpret_cast<const SelfTestEvent*>(event.get())->result);
    CHECK_TRUE(mRecorder->isSignalRecorded(HW_LOCK_CTRL_SERVICE_IS_LOCKED_SIG));
}

TEST(HwLockCtrlServiceTests,
     given_unlocked_when_selftest_request_which_fails_then_service_still_returns_to_unlocked)
{
    using namespace cms::test;

    startServiceToUnlocked();

    auto passed = HW_LOCK_CTRL_SELF_TEST_FAILED_MOTOR;
    mock(HW_LOCK_CTRL_MOCK)
      .expectOneCall("SelfTest")
      .withOutputParameterReturning("outResult", &passed, sizeof(passed));
    mock(HW_LOCK_CTRL_MOCK).expectOneCall("Unlock");
    qf_ctrl::PublishAndProcess(HW_LOCK_CTRL_SERVICE_REQUEST_SELF_TEST_SIG,
                               mRecorder);
    mock().checkExpectations();
    auto event = mRecorder->getRecordedEvent();
    CHECK_TRUE(event != nullptr);
    CHECK_EQUAL(HW_LOCK_CTRL_SERVICE_SELF_TEST_RESULTS_SIG, event->sig);
    CHECK_TRUE(HW_LOCK_CTRL_SELF_TEST_RESULT_FAIL ==
               reinterpret_cast<const SelfTestEvent*>(event.get())->result);
    CHECK_TRUE(
      mRecorder->isSignalRecorded(HW_LOCK_CTRL_SERVICE_IS_UNLOCKED_SIG));
}

TEST(HwLockCtrlServiceTests,
     given_locked_when_10secs_passes_then_service_polls_lock_comm_status)
{
    using namespace cms::test;
    using namespace std::chrono_literals;

    startServiceToLocked();
    mock(HW_LOCK_CTRL_MOCK).expectOneCall("IsCommOk");
    qf_ctrl::MoveTimeForward(10s);
    mock().checkExpectations();
}

TEST(HwLockCtrlServiceTests,
     given_locked_when_9900ms_passes_then_service_has_not_polled_yet)
{
    using namespace cms::test;
    using namespace std::chrono_literals;

    startServiceToLocked();
    mock(HW_LOCK_CTRL_MOCK).expectNoCall("IsCommOk");
    qf_ctrl::MoveTimeForward(9900ms);
    mock().checkExpectations();

    // while here, lets see if it calls the driver upon hitting the 10s mark.
    mock(HW_LOCK_CTRL_MOCK).expectOneCall("IsCommOk");
    qf_ctrl::MoveTimeForward(100ms);
    mock().checkExpectations();
}

TEST(HwLockCtrlServiceTests,
     given_locked_when_60s_passes_then_service_has_polled_six_times)
{
    using namespace cms::test;
    using namespace std::chrono_literals;

    // Demo multiple events over time, and confirm that the
    // service is polling in an ongoing manner.

    startServiceToLocked();
    mock(HW_LOCK_CTRL_MOCK).expectNCalls(6, "IsCommOk");
    qf_ctrl::MoveTimeForward(60s);
    mock().checkExpectations();
}

TEST(HwLockCtrlServiceTests,
     given_test_assert_event_will_assert_and_can_be_tested)
{
    using namespace cms::test;

    static const QEvt assertCausingEvent {
      HW_LOCK_CTRL_DEMONSTRATE_TEST_OF_QASSERT_SIG,
      0,
      0
    };

    startServiceToLocked();

    //The resulting assert, however, causes the active
    //object and its internal HSM to be in an undefined state.
    //we need to skip the destructor/teardown which attempts
    //to process a destroy event, resulting in additional
    //asserts internal to QF.
    skipUnderTestDestructor = true;

    cms::test::MockExpectQAssert();
    QACTIVE_POST(mUnderTest, &assertCausingEvent, 0);

    giveProcessingTime();
    mock().checkExpectations();
}

TEST(HwLockCtrlServiceTests, the_service_responds_to_a_ping_with_a_pong)
{
    using namespace cms::test;

    startServiceToLocked();

    //+500 just to ensure outside any internal private signals
    static constexpr enum_t RESPONSE_SIG = MAX_PUB_SUB_SIG + 500;

    // this test demonstrates testing an AO that must respond directly
    // to an event with a POST directly to an external requesting AO.

    PongEvent pongEvent;
    pongEvent.super.sig = 0;
    pongEvent.source    = nullptr;

    auto dummy = std::unique_ptr<cms::DefaultDummyActiveObject>(
      new cms::DefaultDummyActiveObject());
    dummy->SetPostedEventHandler([&pongEvent](const QEvt* event) {
        auto p              = reinterpret_cast<const PongEvent *>(event);
        pongEvent.super.sig = p->super.sig;
        pongEvent.source    = p->source;
    });

    // Reminder: QF requires that each AO be at a unique priority level
    // hence the '- 1' below.
    dummy->dummyStart(qf_ctrl::UNIT_UNDER_TEST_PRIORITY - 1);

    // Send the Ping to our AO under test and give it
    // some processing time.
    PingEvent pingEvent;
    pingEvent.super.sig = HW_LOCK_CTRL_PING_SIG;
    pingEvent.super.poolId_ = 0;
    pingEvent.super.refCtr_ = 0;
    pingEvent.requester = dummy->getQActive();
    pingEvent.response_sig = RESPONSE_SIG;

    QACTIVE_POST(mUnderTest, &pingEvent.super, 0);
    qf_ctrl::ProcessEvents();

    // confirm that our dummy received a Pong with expected
    // data.
    CHECK_EQUAL(RESPONSE_SIG, pongEvent.super.sig);
    CHECK_EQUAL(mUnderTest, pongEvent.source);
}
