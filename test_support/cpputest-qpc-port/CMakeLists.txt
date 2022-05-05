
set(CPPUTEST_QP_PORT_TOP_DIR ${TEST_SUPPORT_TOP_DIR}/cpputest-qpc-port)
set(CPPUTEST_QP_PORT_SRC_DIR ${CPPUTEST_QP_PORT_TOP_DIR}/src)
set(CPPUTEST_QP_PORT_SRC_FILES
        ${CPPUTEST_QP_PORT_SRC_DIR}/cpputest_qf_port.cpp
        ${CPPUTEST_QP_PORT_SRC_DIR}/cms_cpputest_qf_ctrl.cpp
        ${CPPUTEST_QP_PORT_SRC_DIR}/cms_cpputest_q_onAssert.cpp
        ${CPPUTEST_QP_PORT_SRC_DIR}/cms_cpputest_qf_onCleanup.cpp
        ${QPC_QF_SRCS}
        )

include_directories(${QPC_SRC_DIR})
include_directories(${CPPUTEST_QP_PORT_TOP_DIR}/include)
include_directories(${UTILS_TOP_DIR}/include)

add_subdirectory(test)
add_library(cpputest-qpc-lib  ${CPPUTEST_QP_PORT_SRC_FILES} ../common/cpputestMain.cpp)
target_include_directories(cpputest-qpc-lib PUBLIC
        ${QPC_INCLUDE_DIR}
        ${CPPUTEST_QP_PORT_TOP_DIR}/include
        ${UTILS_TOP_DIR}/include)





