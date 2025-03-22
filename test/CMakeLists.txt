
set(CMAKE_BUILD_TYPE Debug)

list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_LIST_DIR}/../)

find_package(ACADOSCpp REQUIRED)
find_package(GTest REQUIRED)

enable_testing()

add_executable(
  testACADOSCpp
  test.cpp
)

target_link_libraries(
  testACADOSCpp
  GTest::gtest_main
  ACADOSCpp
)

include(GoogleTest)

gtest_discover_tests(testACADOSCpp)
