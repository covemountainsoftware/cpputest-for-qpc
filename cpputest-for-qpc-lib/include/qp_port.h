/// @brief cpputest for qpc, QF port C header file.
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


#ifndef CPPUTEST_FOR_QPC_QF_PORT_H
#define CPPUTEST_FOR_QPC_QF_PORT_H

#include <stdint.h>  // Exact-width types. WG14/N843 C99 Standard
#include <stdbool.h> // Boolean type.      WG14/N843 C99 Standard

#ifdef __cplusplus
extern "C" {
#endif

#define Q_NORETURN  void

#define QACTIVE_EQUEUE_TYPE  QEQueue
#define QACTIVE_THREAD_TYPE  bool

#define QF_EQUEUE_TYPE      QEQueue
#define QF_MAX_ACTIVE       64U
#define QF_MAX_TICK_RATE    2U
#define QACTIVE_CAN_STOP      1
#define QF_EVENT_SIZ_SIZE   4U
#define QF_EQUEUE_CTR_SIZE  2U
#define QF_MPOOL_SIZ_SIZE   4U
#define QF_MPOOL_CTR_SIZE   4U
#define QF_TIMEEVT_CTR_SIZE 4U

//cpputest port, no critical sections are necessary
#define QF_CRIT_STAT
#define QF_CRIT_ENTRY(dummy)
#define QF_CRIT_EXIT(dummy)

#include "qequeue.h"
#include "qmpool.h"
#include "qp.h"

void QF_runUntilNoReadyActiveObjects(void);

/****************************************************************************/
/* interface used only inside QF implementation, but not in applications */
#ifdef QP_IMPL

    #define QF_SCHED_STAT_
    #define QF_SCHED_LOCK_(dummy) ((void)0)
    #define QF_SCHED_UNLOCK_()    ((void)0)
    #define QACTIVE_EQUEUE_WAIT_(me_) \
        Q_ASSERT((me_)->eQueue.frontEvt != (QEvt*)0)
    #define QACTIVE_EQUEUE_SIGNAL_(me_) do { \
        QPSet_insert(&cpputest_readySet_, (me_)->prio); \
    } while (false)

    // native QF event pool operations
    #define QF_EPOOL_TYPE_  QMPool
    #define QF_EPOOL_INIT_(p_, poolSto_, poolSize_, evtSize_) \
        (QMPool_init(&(p_), (poolSto_), (poolSize_), (evtSize_)))
    #define QF_EPOOL_EVENT_SIZE_(p_)  ((uint_fast16_t)(p_).blockSize)
    #define QF_EPOOL_GET_(p_, e_, m_, qsId_) \
        ((e_) = (QEvt *)QMPool_get(&(p_), (m_), (qsId_)))
    #define QF_EPOOL_PUT_(p_, e_, qsId_) \
        (QMPool_put(&(p_), (e_), (qsId_)))

    extern QPSet cpputest_readySet_; /* QV-ready set of active objects */

#endif /* QP_IMPL */

#ifdef __cplusplus
}
#endif

#endif   // CPPUTEST_FOR_QPC_QF_PORT_H
