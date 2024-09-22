/// @brief A dummy active object, useful for testing interactions
///        in a unit test. Same basic idea as QActiveDummy in
///        QUTest, adapted for CppUTest and QP/C (qpc).
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

#ifndef CMS_DUMMY_ACTIVE_OBJECT_HPP
#define CMS_DUMMY_ACTIVE_OBJECT_HPP

#include "qpc.h"
#include <array>
#include <functional>
#include <cstdlib>
#include "cmsVectorBackedQEQueue.hpp"
#include "qevtUniquePtr.hpp"

namespace cms {
namespace test {

/// The Dummy Active Object may be used
/// when an active object (AO) under test is
/// interacting with another AO during a test.
/// This class "is a" QActive (C version)
template <size_t InternalEventCount>
class DummyActiveObject  {
public:

    enum class EventBehavior  {
        CALLBACK,  //original behavior, will call the provided callback
        RECORDER   //will record the event
    };

    using PostedEventHandler = std::function<void(QEvt const*)>;

    DummyActiveObject() :
        m_super(),
        m_eventHandler(nullptr),
        m_incomingEvents(),
        m_behavior(EventBehavior::CALLBACK),
        m_recordedEvents(0)
    {
        m_incomingEvents.fill(nullptr);
        QActive_ctor(&m_super, Q_STATE_CAST(initial));
    }

    explicit DummyActiveObject(EventBehavior behavior) :
        m_super(),
        m_eventHandler(nullptr),
        m_incomingEvents(),
        m_behavior(behavior),
        m_recordedEvents(100)
    {
        m_incomingEvents.fill(nullptr);
        QActive_ctor(&m_super, Q_STATE_CAST(initial));

        if (m_behavior == EventBehavior::RECORDER) {
            m_eventHandler = [=](QEvt const* e) {
                this->RecorderEventHandler(e);
            };
        }
    }

    ~DummyActiveObject()
    {
        QActive_stop(&m_super);
    }

    DummyActiveObject(const DummyActiveObject&)            = delete;
    DummyActiveObject& operator=(const DummyActiveObject&) = delete;
    DummyActiveObject(DummyActiveObject&&)                 = delete;
    DummyActiveObject& operator=(DummyActiveObject&&)      = delete;

    void SetPostedEventHandler(const PostedEventHandler& handler)
    {
        if (m_behavior == EventBehavior::CALLBACK) {
            m_eventHandler = handler;
        }
    }

    void dummyStart(uint_fast8_t priority = 1)
    {
        QACTIVE_START(&m_super, priority,
                      m_incomingEvents.data(), m_incomingEvents.size(),
                      nullptr, 0, nullptr);
    }

    QActive* getQActive()
    {
        return &m_super;
    }

    bool isRecorderEmpty() { return m_recordedEvents.isEmpty(); }

    bool isAnyEventRecorded() { return !m_recordedEvents.isEmpty(); }

    bool isSignalRecorded(enum_t sig)
    {
        if (!isAnyEventRecorded()) {
            return false;
        }

        const auto e       = getRecordedEvent();
        enum_t recordedSig = e->sig;
        return recordedSig == sig;
    }

    cms::QEvtUniquePtr getRecordedEvent()
    {
        if (!isAnyEventRecorded()) {
            return {};
        }

        return cms::QEvtUniquePtr(m_recordedEvents.get());
    }

protected:
    static QState initial(DummyActiveObject* const me,
                              QEvt const* const)
    {
        return Q_TRAN(&running);
    }

    static QState running(DummyActiveObject* const me,
                              QEvt const* const e)
    {
        QState rtn;
        switch (e->sig) {
            case Q_INIT_SIG:
                rtn = Q_SUPER(&QHsm_top);
                break;
            case Q_ENTRY_SIG:   // purposeful fall through
            case Q_EXIT_SIG:
                rtn = Q_HANDLED();
                break;
            default:
                if ((me->m_eventHandler != nullptr) && (e->sig != 0)) {
                    me->m_eventHandler(e);
                }
                rtn = Q_SUPER(&QHsm_top);
                break;
        }

        return rtn;
    }

protected:
    void RecorderEventHandler(QEvt const * e)
    {
        if (e->sig >= Q_USER_SIG) {
            // record the event
            m_recordedEvents.post(e, QF_NO_MARGIN);
        }
    }

private:
    QActive m_super;
    PostedEventHandler m_eventHandler;
    std::array<QEvt const*, InternalEventCount> m_incomingEvents;
    EventBehavior m_behavior;
    cms::VectorBackedQEQueue m_recordedEvents;
};

using DefaultDummyActiveObject = DummyActiveObject<50>;

}  // namespace test
}  // namespace cms

#endif   // CMS_DUMMY_ACTIVE_OBJECT_HPP
