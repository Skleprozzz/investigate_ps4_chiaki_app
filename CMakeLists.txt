
cmake_minimum_required(VERSION 3.2)

project(ps4app)
set (CMAKE_C_STANDARD 99)
option(PS4_ENABLE_TESTS "Enable tests for Ps4App" ON)

add_subdirectory(lib)
add_subdirectory(gui)

if(PS4_ENABLE_TESTS)
	enable_testing()
	add_subdirectory(test)
endif()
