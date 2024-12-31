# ===================
# == Project Setup ==
# ===================

# Ensure our CMake version is met
cmake_minimum_required(VERSION  3.21)

# Ensure we have a valid APP_VERSION_BASE to work with
if (NOT APP_VERSION_BASE)
	set(APP_VERSION_BASE 0.0.0)
endif() # NOT APP_VERSION_BASE

# Setup the test project declaration
project(BriCLI_test
	VERSION ${APP_VERSION_BASE}
	LANGUAGES C CXX
)

# Project options
option(${PROJECT_NAME}_GENERATE_COVERAGE "Enable to build code coverage information from these unit tests" OFF)

# Root directory for finding application files
set(APP_ROOT_DIRECTORY ${PROJECT_SOURCE_DIR}/..)

# ======================
# == Googletest Setup ==
# ======================

include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  DOWNLOAD_EXTRACT_TIMESTAMP ON
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# ===================
# == Test Settings ==
# ===================

include(GoogleTest)
enable_testing()

# Top level target to enable test only builds
add_custom_target(${PROJECT_NAME})

add_executable(ParserTest
  ${APP_ROOT_DIRECTORY}/src/bricli.c
  ${PROJECT_SOURCE_DIR}/ParserTest.cpp
)
target_include_directories(ParserTest PRIVATE 
	${APP_ROOT_DIRECTORY}/inc
	${PROJECT_SOURCE_DIR}/lib
	)
target_link_libraries(ParserTest PRIVATE GTest::gmock_main)
list(APPEND tests ParserTest)

# ====================
# == Test Discovery ==
# ====================
foreach(test ${tests})
	gtest_discover_tests(${test})
	add_dependencies(${PROJECT_NAME} ${test})
	message(STATUS "Found test: ${test}")
endforeach() # (test ${tests})
