#include "cmsDummyActiveObject.hpp"
#include "cms_cpputest_qf_ctrl.hpp"
#include "qpc.h"
#include <memory>

//cpputest header include must always be last
#include "CppUTest/TestHarness.h"

using namespace cms;
using namespace cms::test;

TEST_GROUP(dummy_ao_tests)
{
    void setup() final
    {
        qf_ctrl::Setup(Q_USER_SIG, 100);
    }

    void teardown() final
    {
        qf_ctrl::Teardown();
    }
};

TEST(dummy_ao_tests, dummy_ao_provides_callback_by_default)
{
    auto dummy = std::make_unique<DefaultDummyActiveObject>();
    dummy->dummyStart();
    CHECK_TRUE(dummy->isRecorderEmpty());
    CHECK_FALSE(dummy->isAnyEventRecorded());

    static constexpr enum_t TEST1_SIG = Q_USER_SIG + 1;
    enum_t capturedSig = -1;
    dummy->SetPostedEventHandler([&](const QEvt* e){
        capturedSig = e->sig;
    });
    qf_ctrl::PostAndProcess<TEST1_SIG>(dummy->getQActive());
    CHECK_EQUAL(TEST1_SIG, capturedSig);
    CHECK_FALSE(dummy->isAnyEventRecorded()); //still false, internal recorder not in use here.
    CHECK_TRUE(dummy->isRecorderEmpty());
}

TEST(dummy_ao_tests, dummy_ao_provides_recorder_option)
{
    auto dummy = std::make_unique<DefaultDummyActiveObject>(DefaultDummyActiveObject::EventBehavior::RECORDER);
    dummy->dummyStart();

    static constexpr enum_t TEST1_SIG = Q_USER_SIG + 1;
    static constexpr enum_t TEST2_SIG = TEST1_SIG + 1;

    CHECK_TRUE(dummy->isRecorderEmpty());

    enum_t capturedSig = -1;

    //this should actually do nothing, since this AO is using its internal
    //recorder
    dummy->SetPostedEventHandler([&](const QEvt* e){
        capturedSig = e->sig;
    });
    qf_ctrl::PostAndProcess<TEST1_SIG>(dummy->getQActive());
    qf_ctrl::PostAndProcess<TEST2_SIG>(dummy->getQActive());
    CHECK_EQUAL(-1, capturedSig); //confirm above callback did NOT happen

    CHECK_TRUE(dummy->isAnyEventRecorded());
    auto recordedEvent1 = dummy->getRecordedEvent();
    CHECK_TRUE(recordedEvent1 != nullptr);
    CHECK_EQUAL(TEST1_SIG, recordedEvent1->sig);

    CHECK_TRUE(dummy->isAnyEventRecorded());
    CHECK_TRUE(dummy->isSignalRecorded(TEST2_SIG));
}