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
#include "qf_port.h"      /* QF port */
#include "qf_pkg.h"
#include "qassert.h"
#include <cstring>
#include <cstdint>

Q_DEFINE_THIS_MODULE("cpputest_qf_port")

QPSet  cpputest_readySet_;

extern "C" void QF_init(void)
{
    QF_maxPool_ = static_cast<uint_fast8_t>(0);
    bzero(&QF_timeEvtHead_[0],
          static_cast<uint_fast16_t>(sizeof(QF_timeEvtHead_)));
    bzero(&QF_active_[0], static_cast<uint_fast16_t>(sizeof(QF_active_)));
}

extern "C" void QF_stop(void)
{
    QF_onCleanup();
}

extern "C" void QActive_start_(QActive * const me, uint_fast8_t prio,
                    QEvt const * * const qSto, uint_fast16_t const qLen,
                    void * const stkSto, uint_fast16_t const stkSize,
                    void const * const par)
{
    (void)stkSize; /* unused parameter in this port */

    Q_REQUIRE_ID(600, (0U < prio)  /* priority...*/
                        && (prio <= QF_MAX_ACTIVE) /*... in range */
                        && (stkSto == (void *)0)); /* stack storage must NOT...
                                                    * ... be provided */
    QEQueue_init(&me->eQueue, qSto, qLen);
    me->prio = (uint8_t)prio;
    QF_add_(me); /* make QF aware of this active object */

    /* the top-most initial tran. (virtual) */
    QHSM_INIT(&me->super, par, me->prio);
}

#ifdef QF_ACTIVE_STOP
extern "C" void QActive_stop(QActive * const me) {

    QActive_unsubscribeAll(me); /* unsubscribe from all events */

    /* make sure the AO is no longer in "ready set" */
    QPSet_remove(&cpputest_readySet_, me->prio);

    QF_remove_(me); /* remove this AO from QF */
}
#endif

extern "C" void QF_runUntilNoReadyActiveObjects()
{
    while (QPSet_notEmpty(&cpputest_readySet_)) {
        uint_fast8_t p = QPSet_findMax(&cpputest_readySet_);
        QActive* a     = QF_active_[p];

        // the active object 'a' must still be registered in QF
        // (e.g., it must not be stopped)
        Q_ASSERT_ID(320, a != nullptr);

        while (a->eQueue.frontEvt != nullptr) {
            QEvt const *e = QActive_get_(a);
            QHSM_DISPATCH(&a->super, e, a->prio);
            QF_gc(e);
        }

        if (a->eQueue.frontEvt == nullptr) { /* empty queue? */
            QPSet_remove(&cpputest_readySet_, p);
        }
    }
}