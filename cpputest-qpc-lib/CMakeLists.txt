include_directories(include)

set(CPPUTEST_QP_PORT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories(${QPC_SRC_DIR})
include_directories(${CPPUTEST_QP_PORT_TOP_DIR}/include)

add_subdirectory(tests)
add_library(cpputest-qpc-lib
        src/cpputest_qf_port.cpp
        src/cms_cpputest_qf_ctrl.cpp
        src/cms_cpputest_q_onAssert.cpp
        src/cms_cpputest_qf_onCleanup.cpp
        src/cpputestMain.cpp
        ${QPC_QF_SRCS})

target_include_directories(cpputest-qpc-lib PUBLIC
        ${QPC_INCLUDE_DIR}
        ${CPPUTEST_QP_PORT_TOP_DIR}/include
        ${CMS_UTILS_TOP_DIR}/include)