cmake_minimum_required(VERSION 3.22)
project(Allocators CXX)

set(CMAKE_CXX_STANDARD 23)

# FIXME: Fix naming and provide better cmake modularity
add_library(LML_Allocators INTERFACE)
target_include_directories(LML_Allocators INTERFACE .)

add_executable(Test main.cpp)
target_link_libraries(Test PRIVATE LML_Allocators)target_include_directories(allocators INTERFACE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	include(FetchContent)
	FetchContent_Declare(
			Catch2
			GIT_REPOSITORY https://github.com/catchorg/Catch2.git
			GIT_TAG v3.4.0
			SYSTEM
	)
	FetchContent_MakeAvailable(Catch2)

	file(GLOB_RECURSE TEST_SOURCES "tests/*.test.cpp")

	add_executable(allocator_tests ${TEST_SOURCES})
	target_link_libraries(allocator_tests PRIVATE lml::allocators Catch2::Catch2WithMain)

	enable_testing()
	add_test(NAME AllTests COMMAND allocator_tests)
endif()