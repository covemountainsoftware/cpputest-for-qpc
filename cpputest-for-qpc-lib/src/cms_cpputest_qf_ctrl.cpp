/// @brief Support methods for setup, teardown, and test control of
///        a fake cpputest compatible port of the qpc QF framework.
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

#include "qpc.h"
#include "cms_cpputest_qf_ctrl.hpp"
#include "cmsTestPublishedEventRecorder.hpp"
#include <algorithm>
#include <cassert>
#include <vector>
#include "CppUTest/TestHarness.h"
#include "qassert-meta.h"
#include "cmsQAssertMockSupport.hpp"

namespace cms {
namespace test {
namespace qf_ctrl {

static void CreateDefaultPools();
static void CreatePoolConfigsFromArg(const MemPoolConfigs& pubSubEventMemPoolConfigs);

using SubscriberList                       = std::vector<QSubscrList>;
static SubscriberList* l_subscriberStorage = nullptr;

static uint32_t l_ticksPerSecond = 0;

static MemPoolTeardownOption l_memPoolOption = MemPoolTeardownOption::CHECK_FOR_LEAKS;

struct InternalPoolConfig {
    explicit InternalPoolConfig(const MemPoolConfig& conf) :
        config(conf), storage()
    {
        storage.resize(config.eventSize * config.numberOfEvents);
    }
    MemPoolConfig config;
    std::vector<uint8_t> storage;
};
static std::vector<InternalPoolConfig>* l_pubSubEventMemPoolConfigs = nullptr;

void Setup(enum_t const maxPubSubSignalValue, uint32_t ticksPerSecond,
           const MemPoolConfigs& pubSubEventMemPoolConfigs,
           MemPoolTeardownOption memPoolOpt)
{
    assert(l_subscriberStorage == nullptr);
    assert(l_ticksPerSecond == 0);
    assert(l_pubSubEventMemPoolConfigs == nullptr);

    l_memPoolOption     = memPoolOpt;
    l_ticksPerSecond    = ticksPerSecond;
    l_subscriberStorage = new SubscriberList();
    l_subscriberStorage->resize(static_cast<size_t>(maxPubSubSignalValue));
    QSubscrList nullValue;
    QPSet_setEmpty(&nullValue.set);

#if QP_VERSION >= 800U
    //safety fields eliminated from GPL version of QP starting in v8.x.x
#elif QP_VERSION > 700U
    QPSet_setEmpty(&nullValue.set_dis);
#else
    #error "unsupported QP version"
#endif

    std::fill(l_subscriberStorage->begin(), l_subscriberStorage->end(),
              nullValue);

    if (pubSubEventMemPoolConfigs.empty()) {
        CreateDefaultPools();
    }
    else {
        CreatePoolConfigsFromArg(pubSubEventMemPoolConfigs);
    }

    QAssertMetaInit();
    QAssertMetaOutputEnable();

    QF_init();
    QF_psInit(l_subscriberStorage->data(), maxPubSubSignalValue);

    for (auto& config : *l_pubSubEventMemPoolConfigs) {
        QF_poolInit(config.storage.data(), config.storage.size(),
                     config.config.eventSize);
    }
}

void Teardown()
{
    delete l_subscriberStorage;
    l_subscriberStorage = nullptr;

    l_ticksPerSecond = 0;

    // No test should complete with allocated events sitting
    // in a memory pool. However, the QP Pool class
    // is rather private. Ideally that
    // QP class is modified to include size/usage accessors.
    // Instead, we modified the QF port stop() method, which
    // internally will perform the memory check, as it has
    // friend access.
    if (l_memPoolOption == MemPoolTeardownOption::CHECK_FOR_LEAKS) {
        QF_stop();
    }

    if (l_pubSubEventMemPoolConfigs != nullptr) {
        delete l_pubSubEventMemPoolConfigs;
        l_pubSubEventMemPoolConfigs = nullptr;
    }
}

void ChangeMemPoolTeardownOption(MemPoolTeardownOption memPoolOpt)
{
    l_memPoolOption = memPoolOpt;
}

void ProcessEvents()
{
    QF_runUntilNoReadyActiveObjects();
}

void MoveTimeForward(const std::chrono::milliseconds& duration)
{
    assert(l_ticksPerSecond != 0);
    assert(duration.count() >= 0);

    double millisecondsPerTick = 1000.0 / l_ticksPerSecond;

    using LoopCounter_t = uint64_t;

    constexpr LoopCounter_t ONCE = 1;
    // if called, ensure at least one tick is processed
    LoopCounter_t ticks = std::max(
      ONCE, static_cast<LoopCounter_t>(duration.count() / millisecondsPerTick));

    for (LoopCounter_t i = 0; i < ticks; ++i) {
        QTIMEEVT_TICK_X(0U, nullptr);
        ProcessEvents();
    }
}

void PublishEvent(enum_t sig)
{
    auto e = Q_NEW(QEvt, sig);
    QF_PUBLISH(e, 0);
}

void PublishEvent(QEvt const* const e)
{
    QF_PUBLISH(e, nullptr);
}

void PublishAndProcess(enum_t sig, PublishedEventRecorder* recorder)
{
    if (recorder != nullptr) {
        recorder->oneShotIgnoreEvent(sig);
    }

    PublishEvent(sig);
    ProcessEvents();
}

void PublishAndProcess(QEvt const* const e,
                       PublishedEventRecorder* recorder)
{
    if (recorder != nullptr) {
        recorder->oneShotIgnoreEvent(e->sig);
    }

    PublishEvent(e);
    ProcessEvents();
}

void CreateDefaultPools()
{
    l_pubSubEventMemPoolConfigs = new std::vector<InternalPoolConfig>();
    l_pubSubEventMemPoolConfigs->push_back(
      InternalPoolConfig(MemPoolConfig {sizeof(uint64_t), 25}));
    l_pubSubEventMemPoolConfigs->push_back(
      InternalPoolConfig(MemPoolConfig {sizeof(uint64_t) * 5, 10}));
    l_pubSubEventMemPoolConfigs->push_back(
      InternalPoolConfig(MemPoolConfig {sizeof(uint64_t) * 15, 5}));
}

void CreatePoolConfigsFromArg(const MemPoolConfigs& pubSubEventMemPoolConfigs)
{
    l_pubSubEventMemPoolConfigs = new std::vector<InternalPoolConfig>();
    for (const auto& config : pubSubEventMemPoolConfigs) {
        l_pubSubEventMemPoolConfigs->push_back(InternalPoolConfig(config));
    }
}

const char * GetVersion()
{
    return CPPUTEST_FOR_QPC_LIB_VERSION;
}

}   // namespace qf_ctrl
}   // namespace test
}   // namespace cms
