
set(CMAKE_BUILD_TYPE Debug)

find_package(GTest REQUIRED)

enable_testing()

add_executable(
  testACADOSCpp
  test.cpp
)

target_link_libraries(
  testACADOSCpp
  GTest::gtest_main
  acados
)

include(GoogleTest)

gtest_discover_tests(testACADOSCpp)
