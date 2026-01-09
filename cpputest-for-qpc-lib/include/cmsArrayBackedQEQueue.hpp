/// @brief Utility class to simplify creating QEQueue objects.
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

#ifndef CMS_ARRAY_BACKED_QEQUEUE_HPP
#define CMS_ARRAY_BACKED_QEQUEUE_HPP

#include "qpc.h"
#include <cstdint>
#include <array>

namespace cms {

/// The ArrayBacked QEQueue provides a simplified
/// approach to associating static storage with a QEQueue
/// \tparam MaxEvents
template <std::uint_fast16_t MaxEvents>
class ArrayBackedQEQueue  {
public:
    ArrayBackedQEQueue() :
        m_queue(),
        m_storage()
    {
        m_storage.fill(nullptr);
        QEQueue_init(&m_queue, m_storage.data(), MaxEvents);
    }

    size_t capacity() const
    {
        return m_storage.size() + 1;   // see QEQueue docs for +1 reason
    }

    bool post(QEvt const * const e,
                       std::uint_fast16_t const margin) noexcept
    {
        return QEQueue_post(&m_queue, e, margin, 0);
    }

    bool isEmpty()
    {
        return QEQueue_isEmpty(&m_queue);
    }

    QEQueueCtr getNFree()
    {
        #if QP_VERSION < 810
        return QEQueue_getNFree(&m_queue);
        #else
        return QEQueue_getFree(&m_queue);
        #endif
    }

    QEvt const * get()
    {
        return QEQueue_get(&m_queue, 0);
    }

private:
    QEQueue m_queue;
    std::array<QEvt const *, MaxEvents> m_storage;
};

}   // namespace cms

#endif   // CMS_ARRAY_BACKED_QEQUEUE_HPP
