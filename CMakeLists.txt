cmake_minimum_required(VERSION 3.16)
project(cpputest-for-qpc-lib VERSION 1.0.1)
include(FetchContent)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 11)

add_compile_options(-Wall -Wextra -Werror)

set(CMS_EXTERNALS_TOP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/externals)
set(CMS_CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cpputest-for-qpc-lib/cmake CACHE INTERNAL "")

if(NOT DEFINED CMS_QPC_TOP_DIR)
    set(CMS_QPC_TOP_DIR ${CMS_EXTERNALS_TOP_DIR}/qpc)
    FetchContent_Declare(qpc
            GIT_REPOSITORY https://github.com/QuantumLeaps/qpc.git
            GIT_TAG        fcea9943bbeeca49c66ce124d4d71467f6e2661e #7.3.3
            SOURCE_DIR     ${CMS_QPC_TOP_DIR}
    )
    message("Fetching QP/C git repository")
    FetchContent_MakeAvailable(qpc)
endif(NOT DEFINED CMS_QPC_TOP_DIR)

include(${CMS_CMAKE_DIR}/qpcCMakeSupport.cmake)
add_subdirectory(cpputest-for-qpc-lib)
