cmake_minimum_required(VERSION 3.12)

project(ACADOSCpp)

option(ACADOS_CPP_BUILD_TESTS "Build tests." OFF)

include(cmake/FindACADOS.cmake)

include_directories(${CMAKE_CURRENT_LIST_DIR}/include ${ACADOS_INCLUDE_DIRECTORIES})

link_directories(
    ${ACADOS_LINK_DIRECTORIES}
)

if(ACADOS_CPP_BUILD_TESTS)
    add_subdirectory(test/)
endif()
