cmake_minimum_required(VERSION 3.16)
project(cpputest-for-qpc-lib VERSION 1.3.0)
include(FetchContent)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

add_compile_options(-Wall -Wextra -Werror)

set(CMS_EXTERNALS_TOP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/externals)
set(CMS_CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cpputest-for-qpc-lib/cmake CACHE INTERNAL "")
set(CMS_QASSERT_META_TOP_DIR ${CMS_EXTERNALS_TOP_DIR}/qassert-meta)

if(NOT DEFINED CMS_QPC_TOP_DIR)
    set(CMS_QPC_TOP_DIR ${CMS_EXTERNALS_TOP_DIR}/qpc)
    FetchContent_Declare(qpc
            GIT_REPOSITORY https://github.com/QuantumLeaps/qpc.git
            GIT_TAG        6cbb42cf26f10e8ba1fff887f5fd66f674b2d92a #8.0.4
            SOURCE_DIR     ${CMS_QPC_TOP_DIR}
    )
    message("Fetching QP/C git repository")
    FetchContent_MakeAvailable(qpc)
endif(NOT DEFINED CMS_QPC_TOP_DIR)

FetchContent_Declare(qassert-meta
        GIT_REPOSITORY https://github.com/covemountainsoftware/qassert-meta.git
        GIT_TAG        a53c87e1a351f9c0444fcd2c68bb0583e2109b11
        SOURCE_DIR     ${CMS_QASSERT_META_TOP_DIR}
)
message("Fetching qassert-meta git repository")
FetchContent_MakeAvailable(qassert-meta)

include(${CMS_CMAKE_DIR}/qpcCMakeSupport.cmake)
add_subdirectory(cpputest-for-qpc-lib)
