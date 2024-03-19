/// @brief A sample/demonstration QP/C QActive based service used
///        to demonstrate host based unit testing of an active object
///        using cpputest and the cpputest-for-qpc support code.
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

#include "hwLockCtrlService.h"
#include "hwLockCtrl.h"
#include "hwLockCtrlSelfTestEvent.h"
#include "pingPongEvents.h"
#include "bspTicks.h"
#include "qsafe.h"
#include <stddef.h>

Q_DEFINE_THIS_FILE

// 10 second polling rate
static const uint32_t TICKS_PER_POLL =  BSP_TICKS_PER_SECOND * 10;

/**
 * The state of the HW Lock controlled
 * by this service.
 */
typedef enum {
    LOCK_STATE_UNKNOWN,
    LOCK_STATE_LOCKED,
    LOCK_STATE_UNLOCKED,
} HwLockCtrlLockState;

/**
 * Internal signals
 */
enum InternalSignals {
    REQUEST_GOTO_HISTORY_SIG = HW_LOCK_CTRL_MAX_DIRECT_SIG_,
    POLL_COMM_STATUS_SIG,
    SERVICE_DESTRUCTOR_SIG
};

typedef struct {
    QActive       super; /* inherit QActive, via QP/C Framework C style */

    QStateHandler history;
    QTimeEvt      pollingTimer;
    bool          isRunning;
} HwLockCtrlService;

//HwLockCtrlService state handlers
static QState initial(HwLockCtrlService * me, void const * par);
static QState common_state(HwLockCtrlService * me, QEvt const * e);
static QState   state_of_locked(HwLockCtrlService * me, QEvt const * e);
static QState   state_of_self_test(HwLockCtrlService * me, QEvt const * e);
static QState   state_of_unlocked(HwLockCtrlService * me, QEvt const * e);
static QState state_of_destroyed(HwLockCtrlService * me, QEvt const * e);

//internal helper methods
static void PerformSelfTest(HwLockCtrlService * me);
static void NotifyChangedState(HwLockCtrlService * me, HwLockCtrlLockState state);
static void NotifySelfTestResult(HwLockCtrlService * me, SelfTestResult result);

static HwLockCtrlService m_instance;

QActive * g_theHwLockCtrlService = NULL;

void HwLockCtrlService_ctor()
{
    static const HwLockCtrlService defaultServiceValue;
    m_instance = defaultServiceValue;

    Q_ASSERT(g_theHwLockCtrlService == NULL);
    Q_ASSERT(m_instance.isRunning == false);
    QActive_ctor(&m_instance.super, Q_STATE_CAST(initial));
    m_instance.history = NULL;
    g_theHwLockCtrlService = &m_instance.super;
}

void HwLockCtrlService_dtor()
{
    static QEvt const selfDestructEvent = QEVT_INITIALIZER(SERVICE_DESTRUCTOR_SIG);

    Q_ASSERT(g_theHwLockCtrlService != NULL);

    //check if the AO was actually started before
    //posting the self-destruct event
    if (m_instance.isRunning)
    {
        QACTIVE_POST(&m_instance.super, &selfDestructEvent, NULL);
    }

    g_theHwLockCtrlService = NULL;
}

static QState initial(HwLockCtrlService * const me, void const * const par)
{
    (void)par;
    HwLockCtrlInit();
    QTimeEvt_ctorX(&me->pollingTimer, &me->super, POLL_COMM_STATUS_SIG, 0U);
    QActive_subscribe(&me->super, HW_LOCK_CTRL_SERVICE_REQUEST_SELF_TEST_SIG);
    QActive_subscribe(&me->super,HW_LOCK_CTRL_SERVICE_REQUEST_LOCKED_SIG);
    QActive_subscribe(&me->super,HW_LOCK_CTRL_SERVICE_REQUEST_UNLOCKED_SIG);
    me->isRunning = true;
    return Q_TRAN(&common_state);
}

QState common_state(HwLockCtrlService * me, const QEvt* e)
{
    QState rtn;
    switch (e->sig) {
        case Q_INIT_SIG:
            rtn = Q_TRAN(state_of_locked);
            break;
        case Q_ENTRY_SIG:
            QTimeEvt_armX(&me->pollingTimer,
                          TICKS_PER_POLL, TICKS_PER_POLL);
            rtn = Q_RET_HANDLED;
            break;
        case Q_EXIT_SIG:
            QTimeEvt_disarm(&me->pollingTimer);
            rtn = Q_RET_HANDLED;
            break;
        case HW_LOCK_CTRL_SERVICE_REQUEST_SELF_TEST_SIG:
            rtn = Q_TRAN(state_of_self_test);
            break;
        case HW_LOCK_CTRL_SERVICE_REQUEST_LOCKED_SIG:
            rtn = Q_TRAN(state_of_locked);
            break;
        case HW_LOCK_CTRL_SERVICE_REQUEST_UNLOCKED_SIG:
            rtn = Q_TRAN(state_of_unlocked);
            break;
        case POLL_COMM_STATUS_SIG:
            // this is just to demonstrate unit testing of time
            // based functionality. Perhaps a real module would
            //  publish a result if this fails, etc.
            HwLockCtrlIsCommOk();
            rtn = Q_RET_HANDLED;
            break;
        case HW_LOCK_CTRL_DEMONSTRATE_TEST_OF_QASSERT_SIG:
            // demonstrate testing assertions from cpputest
            Q_ASSERT(true == false);
            rtn = Q_RET_HANDLED;
            break;
        case HW_LOCK_CTRL_PING_SIG: {
            const PingEvent* ping = (const PingEvent*)e;

            PongEvent * pong = Q_NEW(PongEvent, ping->response_sig);
            pong->source = g_theHwLockCtrlService;
            QACTIVE_POST(ping->requester, &pong->super, g_theHwLockCtrlService);
            rtn = Q_RET_HANDLED;
        } break;
        case SERVICE_DESTRUCTOR_SIG:
            rtn = Q_TRAN(state_of_destroyed);
            break;
        default:
            rtn = Q_SUPER(&QHsm_top);
            break;
    }

    return rtn;
}

QState state_of_locked(HwLockCtrlService * me, const QEvt* e)
{
    QState rtn;
    switch (e->sig) {
        case Q_ENTRY_SIG:
            HwLockCtrlLock();
            NotifyChangedState(me, LOCK_STATE_LOCKED);
            rtn = Q_RET_HANDLED;
            break;
        case Q_EXIT_SIG:
            rtn = Q_RET_HANDLED;
            break;
        case HW_LOCK_CTRL_SERVICE_REQUEST_LOCKED_SIG:
            rtn = Q_RET_HANDLED;
            break;
        default:
            rtn = Q_SUPER(&common_state);
            break;
    }

    return rtn;
}

QState state_of_unlocked(HwLockCtrlService * me, const QEvt* e)
{
    QState rtn;
    switch (e->sig) {
        case Q_ENTRY_SIG:
            HwLockCtrlUnlock();
            NotifyChangedState(me, LOCK_STATE_UNLOCKED);
            rtn = Q_RET_HANDLED;
            break;
        case Q_EXIT_SIG:
            rtn = Q_RET_HANDLED;
            break;
        case HW_LOCK_CTRL_SERVICE_REQUEST_UNLOCKED_SIG:
            rtn = Q_RET_HANDLED;
            break;
        default:
            rtn = Q_SUPER(&common_state);
            break;
    }

    return rtn;
}

QState state_of_self_test(HwLockCtrlService * me, const QEvt* e)
{
    QState rtn;
    switch (e->sig) {
        case Q_ENTRY_SIG:
            PerformSelfTest(me);
            rtn = Q_RET_HANDLED;
            break;
        case REQUEST_GOTO_HISTORY_SIG:
            rtn = Q_TRAN(me->history);
            break;
        case HW_LOCK_CTRL_SERVICE_REQUEST_SELF_TEST_SIG:
            rtn = Q_RET_HANDLED;
            break;
        case POLL_COMM_STATUS_SIG:
            // drop during self test
            rtn = Q_RET_HANDLED;
            break;
        default:
            rtn = Q_SUPER(&common_state);
            break;
    }

    return rtn;
}

QState state_of_destroyed(HwLockCtrlService * me, const QEvt* e)
{
    QState rtn;
    switch (e->sig) {
        case Q_ENTRY_SIG:
            QActive_stop(&me->super);
            me->isRunning = false;
            rtn = Q_RET_HANDLED;
            break;
        case Q_EXIT_SIG:
            Q_ASSERT(true == false); //impossible to exit or process any events
            rtn = Q_RET_HANDLED;
            break;
        default:
            rtn = Q_SUPER(&QHsm_top);
            break;
    }

    return rtn;
}

void PerformSelfTest(HwLockCtrlService * const me)
{
    HwLockCtrlSelfTestResultT result;
    bool ok = HwLockCtrlSelfTest(&result);
    if (ok && (result == HW_LOCK_CTRL_SELF_TEST_PASSED)) {
        NotifySelfTestResult(me, HW_LOCK_CTRL_SELF_TEST_RESULT_PASS);
    }
    else {
        NotifySelfTestResult(me, HW_LOCK_CTRL_SELF_TEST_RESULT_FAIL);
    }

    // remind self to transition back to
    // history per this service's requirements
    // note the use of "postLIFO" (urgent) as per:
    //
    //  https://covemountainsoftware.com/2020/03/08/uml-statechart-handling-errors-when-entering-a-state/
    //
    static QEvt const event = QEVT_INITIALIZER(REQUEST_GOTO_HISTORY_SIG);

    QACTIVE_POST_LIFO(&me->super, &event);
}

void NotifyChangedState(HwLockCtrlService * const me, HwLockCtrlLockState state)
{
    (void)me; //the QF_PUBLISH macro may not be using that member
              //in some build configurations

    static QEvt const lockedEvent = QEVT_INITIALIZER(HW_LOCK_CTRL_SERVICE_IS_LOCKED_SIG);
    static QEvt const unlockedEvent = QEVT_INITIALIZER(HW_LOCK_CTRL_SERVICE_IS_UNLOCKED_SIG);

    switch (state) {
        case LOCK_STATE_LOCKED:
            QF_PUBLISH(&lockedEvent, &me->super);
            m_instance.history = Q_STATE_CAST(&state_of_locked);
            break;
        case LOCK_STATE_UNLOCKED:
            QF_PUBLISH(&unlockedEvent, &me->super);
            m_instance.history = Q_STATE_CAST(&state_of_unlocked);
            break;
        default:
            Q_ASSERT(true == false);
            break;
    }
}

void NotifySelfTestResult(HwLockCtrlService * const me, SelfTestResult result)
{
    (void)me; //the QF_PUBLISH macro may not be using that member
              //in some build configurations

    switch (result) {
        case HW_LOCK_CTRL_SELF_TEST_RESULT_PASS:   // fall through on purpose
        case HW_LOCK_CTRL_SELF_TEST_RESULT_FAIL: {
            SelfTestEvent* e = Q_NEW(SelfTestEvent, HW_LOCK_CTRL_SERVICE_SELF_TEST_RESULTS_SIG);
            e->result = result;
            QF_PUBLISH(&e->super, &me->super);
        }
            break;
        default:
            Q_ASSERT(true == false);
            break;
    }
}
