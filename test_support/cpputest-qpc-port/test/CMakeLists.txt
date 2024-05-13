
set(TEST_APP_NAME cpputest-qpc-portTests)
set(TEST_SOURCES
        qassertTests.cpp
        cms_cpputest_qf_ctrlTests.cpp
        cms_cpputest_qf_ctrlPublishTests.cpp
        publishedEventRecorderTests.cpp
        )

# this include expects TEST_SOURCES and TEST_APP_NAME to be
# defined, and creates the cpputest based test executable target
include(${CMS_TEST_SUPPORT_TOP_DIR}/common/cpputestCMake.txt)

target_link_libraries(${TEST_APP_NAME} cpputest-qpc-lib  ${CPPUTEST_LDFLAGS})