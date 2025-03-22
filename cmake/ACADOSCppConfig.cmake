
message(STATUS "Found ACADOSCpp. ${CMAKE_CURRENT_LIST_DIR}")

set(ACADOSCPP_FOUND TRUE)

find_package(acados REQUIRED)


add_library(ACADOSCpp INTERFACE)
target_include_directories(ACADOSCpp INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(ACADOSCpp INTERFACE acados dl)