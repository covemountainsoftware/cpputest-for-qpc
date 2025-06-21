/// @brief   A fake cpputest compatible port of the qpc QF framework,
///          enabling host based unit testing of active objects based
///          on the qpc framework.
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
#include "qp_port.h"
#include "qsafe.h"
#include "qp_pkg.h"
#include <cstring>

Q_DEFINE_THIS_MODULE("cpputest_qf_port")

QPSet  cpputest_readySet_;

extern "C" void QF_init(void)
{
    QF_priv_.maxPool_ = static_cast<uint_fast8_t>(0);
    for (uint_fast8_t tickRate = 0U;
         tickRate < Q_DIM(QTimeEvt_timeEvtHead_);
         ++tickRate)
    {
        QTimeEvt_ctorX(&QTimeEvt_timeEvtHead_[tickRate],
                       nullptr, Q_USER_SIG, tickRate);
    }

    bzero(&QActive_registry_[0], static_cast<uint_fast16_t>(sizeof(QActive_registry_)));
}

extern "C" void QF_stop(void)
{
    QF_onCleanup();
}

#if QP_VERSION >= 800U
extern "C" void QActive_start(QActive * const me,
                               QPrioSpec const prioSpec,
                               QEvt const * * const qSto,
                               uint_fast16_t const qLen,
                               void * const stkSto,
                               uint_fast16_t const stkSize,
                               void const * const par)

#elif QP_VERSION > 700U
extern "C" void QActive_start_(QActive * const me,
                               QPrioSpec const prioSpec,
                               QEvt const * * const qSto,
                               uint_fast16_t const qLen,
                               void * const stkSto,
                               uint_fast16_t const stkSize,
                               void const * const par)
#else
#error "unsupported QP version"
#endif
{
    Q_UNUSED_PAR(stkSto);
    Q_UNUSED_PAR(stkSize);

    QEQueue_init(&me->eQueue, qSto, qLen);
    me->prio  = (uint8_t)(prioSpec & 0xFFU); // QF-priority of the AO

    QActive_register_(me); // register this AO

    // top-most initial tran. (virtual call)
    (*me->super.vptr->init)(&me->super, par, me->prio);
}

#ifdef QACTIVE_CAN_STOP
extern "C" void QActive_stop(QActive * const me) {

    QActive_unsubscribeAll(me); /* unsubscribe from all events */

    /* make sure the AO is no longer in "ready set" */
    QPSet_remove(&cpputest_readySet_, me->prio);

    QActive_unregister_(me); // un-register this active object
}
#endif

extern "C" void QF_runUntilNoReadyActiveObjects()
{
    while (QPSet_notEmpty(&cpputest_readySet_)) {
        uint_fast8_t p = QPSet_findMax(&cpputest_readySet_);
        QActive* a     = QActive_registry_[p];

        // the active object 'a' must still be registered in QF
        // (e.g., it must not be stopped)
        Q_ASSERT_ID(320, a != nullptr);

        while (a->eQueue.frontEvt != nullptr) {
            QEvt const *e = QActive_get_(a);
            (*a->super.vptr->dispatch)(&a->super, e, a->prio);
            QF_gc(e);
        }

        if (a->eQueue.frontEvt == nullptr) { /* empty queue? */
            QPSet_remove(&cpputest_readySet_, p);
        }
    }
}