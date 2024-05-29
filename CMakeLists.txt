cmake_minimum_required(VERSION 3.16)
project(cpputest-for-qpc-lib)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 11)

add_compile_options(-Wall -Wextra -Werror)

set(CMS_EXTERNALS_TOP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/externals)
set(CMS_CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cpputest-for-qpc-lib/cmake)
set(CMS_QPC_TOP_DIR ${CMS_EXTERNALS_TOP_DIR}/qpc)

include(${CMS_CMAKE_DIR}/qpcCMakeSupport.cmake)

# todo: if not set then use externals
set(CMS_QPC_TOP_DIR ${EXTERNALS_TOP_DIR}/qpc)

add_subdirectory(cpputest-for-qpc-lib)
