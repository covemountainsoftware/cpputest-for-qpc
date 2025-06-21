include_directories(include)
include_directories(${CMS_QPC_SRC_DIR})
include_directories(${CMS_CPPUTEST_QP_PORT_TOP_DIR}/include)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_definitions(-DCPPUTEST_FOR_QPC_LIB_VERSION=\"${cpputest-for-qpc-lib_VERSION}\")

add_library(cpputest-for-qpc-lib
        src/cpputest_qf_port.cpp
        src/cms_cpputest_qf_ctrl.cpp
        src/cms_cpputest_q_onAssert.cpp
        src/cms_cpputest_qf_onCleanup.cpp
        src/cpputestMain.cpp
        ${CMS_QPC_QF_SRCS})
target_link_libraries(cpputest-for-qpc-lib qassert-meta-lib)
add_subdirectory(tests)

target_compile_options(cpputest-for-qpc-lib PRIVATE -Wall -Wextra -Werror -Wpedantic -Wsign-conversion)

target_include_directories(cpputest-for-qpc-lib PUBLIC
        ${CMS_QPC_INCLUDE_DIR}
        include)
