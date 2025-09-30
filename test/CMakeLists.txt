cmake_minimum_required(VERSION 3.11)

# ---- Project Settings ----
project(BriCLI VERSION 1.0
                     DESCRIPTION "A lightweight C Command Line Interface"
                     LANGUAGES C CXX)

# GoogleTest requires at least C++11
set(CMAKE_CXX_STANDARD 11)

# Build flags to enable code coverage tests.
SET(GCC_COVERAGE_COMPILE_FLAGS -fprofile-arcs -ftest-coverage -Wall -DUNIT_TEST)
SET(GCC_COVERAGE_LINK_FLAGS    -fprofile-arcs -lgcov)

# Set up directory paths
set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/../source)
set(INC_DIR ${CMAKE_CURRENT_LIST_DIR}/../include ${CMAKE_CURRENT_LIST_DIR}/../config)
set(LIB_DIR ${CMAKE_CURRENT_LIST_DIR}/lib)
set(TEST_DIR ${CMAKE_CURRENT_LIST_DIR})
set(MOCK_DIR ${TEST_DIR}/mocks)

# Add the GoogleTest library
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# ---- Configure Tetsting ----
enable_testing()

# Enable debug symbols
set(CMAKE_BUILD_TYPE RelWithDebInfo)

# Add the auth test.
add_executable(auth-test
    ${SRC_DIR}/bricli.c
    ${TEST_DIR}/TestAuth.cpp
)
target_include_directories(auth-test PUBLIC ${INC_DIR} ${LIB_DIR} ${SRC_DIR})
target_link_libraries(auth-test GTest::gtest_main)
target_compile_options(auth-test PRIVATE ${GCC_COVERAGE_COMPILE_FLAGS})
target_link_options(auth-test PRIVATE ${GCC_COVERAGE_LINK_FLAGS})

# Add the receive test.
add_executable(receive-test
    ${SRC_DIR}/bricli.c
    ${TEST_DIR}/TestReceive.cpp
)
target_include_directories(receive-test PUBLIC ${INC_DIR} ${LIB_DIR} ${SRC_DIR})
target_link_libraries(receive-test GTest::gtest_main)
target_compile_options(receive-test PRIVATE ${GCC_COVERAGE_COMPILE_FLAGS})
target_link_options(receive-test PRIVATE ${GCC_COVERAGE_LINK_FLAGS})

# Add the send test.
add_executable(send-test
    ${SRC_DIR}/bricli.c
    ${TEST_DIR}/TestSend.cpp
)
target_include_directories(send-test PUBLIC ${INC_DIR} ${LIB_DIR} ${SRC_DIR})
target_link_libraries(send-test GTest::gtest_main)
target_compile_options(send-test PRIVATE ${GCC_COVERAGE_COMPILE_FLAGS})
target_link_options(send-test PRIVATE ${GCC_COVERAGE_LINK_FLAGS})

# Add the handlers test.
add_executable(handler-test
    ${SRC_DIR}/bricli.c
    ${TEST_DIR}/TestHandlers.cpp
)
target_include_directories(handler-test PUBLIC ${INC_DIR} ${LIB_DIR} ${SRC_DIR})
target_link_libraries(handler-test GTest::gtest_main)
target_compile_options(handler-test PRIVATE ${GCC_COVERAGE_COMPILE_FLAGS})
target_link_options(handler-test PRIVATE ${GCC_COVERAGE_LINK_FLAGS})

# ---- Discover all GoogleTest binaries ----
include(GoogleTest)
gtest_discover_tests(auth-test PROPERTIES TEST_LIST unitTests)
gtest_discover_tests(receive-test PROPERTIES TEST_LIST unitTests)
gtest_discover_tests(send-test PROPERTIES TEST_LIST unitTests)
gtest_discover_tests(handler-test PROPERTIES TEST_LIST unitTests)

# Ensure all our tests will return a negative error code on failure
set_tests_properties(${unitTests} PROPERTIES WILL_FAIL TRUE)