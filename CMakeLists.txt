cmake_minimum_required(VERSION 3.12)

project(ACADOSCpp)

include(FindACADOS.cmake)

include_directories(${CMAKE_CURRENT_LIST_DIR}/include ${ACADOS_INCLUDE_DIRECTORIES})

add_library(
    ACADOSCpp
    src/Capsule.cpp
)

target_link_libraries(
    ACADOSCpp
    INTERFACE
    ${ACADOS_LIBS}    
)

link_directories(
    ${ACADOS_LINK_DIRECTORIES}
)

add_subdirectory(test/)
