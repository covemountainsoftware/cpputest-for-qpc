cmake_minimum_required(VERSION 3.16)
project(cpputest-for-qpc)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 11)

add_compile_options(-Wall -Wextra -Werror)

include_directories(include)

set(EXTERNALS_TOP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/externals)
set(CMS_QPC_TOP_DIR ${EXTERNALS_TOP_DIR}/qpc)

include(externals/qpcCMakeSupport.txt)

set(CMS_TEST_SUPPORT_TOP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/test_support)
set(MOCKS_TOP_DIR ${CMS_TEST_SUPPORT_TOP_DIR}/mocks)
set(DRIVERS_TOP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/drivers)
set(CMS_UTILS_TOP_DIR ${CMAKE_CURRENT_SOURCE_DIR}/utilities)

add_subdirectory(utilities)
add_subdirectory(test_support)
add_subdirectory(drivers)
add_subdirectory(examples)
