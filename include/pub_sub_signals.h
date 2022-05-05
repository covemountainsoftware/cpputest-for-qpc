///***************************************************************************
/// @brief The public signals used in the publish/subscribe event system
///        of this QP/C (qpc) based project. These signals are expected
///        to be shared across multiple active objects.
///***************************************************************************

#ifndef PUB_SUB_SIGNALS_H
#define PUB_SUB_SIGNALS_H

#include "qpc.h"

#ifdef __cplusplus
extern "C" {
#endif

/// The publish/subscribe signals allocated for this project
enum PubSubSignals {
    STARTING_PUB_SUB_SIG = Q_USER_SIG,

    #include "hwLockCtrlServiceExpectedPubSubSigs.h"

    // The last published signal enum value. Note:
    // active objects should start their internal
    // private signal enums values after this value.
    MAX_PUB_SUB_SIG
};

#ifdef __cplusplus
}
#endif

#endif   // PUB_SUB_SIGNALS_H
