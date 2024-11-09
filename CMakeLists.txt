cmake_minimum_required(VERSION 3.12)

project(ACADOSCpp)

option(ACADOS_CPP_BUILD_TESTS "Build tests." OFF)

include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

find_package(acados REQUIRED)

if(ACADOS_CPP_BUILD_TESTS)
    add_subdirectory(test/)
endif()
