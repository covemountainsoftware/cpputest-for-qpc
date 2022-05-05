/// @brief Trivial 'Ping' and 'Pong' event types, used to demonstrate
///        direct post and response events to/from an active object.
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
#ifndef PING_PONG_EVENTS_H
#define PING_PONG_EVENTS_H

#include "qpc.h"

#ifdef __cplusplus
extern "C" {
#endif

/// "PingEvent" - a trivial ping event. An active object
///          supporting this event is expected
///          to respond with a Pong event, using
///          the provided requester and response sig.
///
typedef struct  {
    QEvt     super; /* inherit QEvt, via QP/C Framework C style */

    QActive* requester;    //the active object making this request
    enum_t   response_sig; //the sig value that should be used in the Pong response
} PingEvent;

/// "PongEvent" - a trivial pong event, sent in response of
///          an active object that supports a Ping
///          event.
typedef struct {
    QEvt     super; /* inherit QEvt, via QP/C Framework C style */

    QActive* source;  //the active object that sent this PongEvent
} PongEvent;

#ifdef __cplusplus
}
#endif

#endif   // PING_PONG_EVENTS_H
