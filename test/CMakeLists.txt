
include(FetchContent)

set(CMAKE_BUILD_TYPE Debug)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)


enable_testing()

add_executable(
  testACADOSCpp
  test.cpp
)

target_link_libraries(
  testACADOSCpp
  GTest::gtest_main
  ${ACADOS_LIBS}
)

include(GoogleTest)

gtest_discover_tests(testACADOSCpp)
