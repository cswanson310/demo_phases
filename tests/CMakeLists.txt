include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG main
)

# Prevent overriding the parent project's compiler/linker settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

add_executable(unit_tests test_lib.cpp)
target_link_libraries(unit_tests PRIVATE gtest_main toy_lib)

include(GoogleTest)

gtest_discover_tests(unit_tests)
