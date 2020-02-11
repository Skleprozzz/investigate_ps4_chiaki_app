
cmake_minimum_required(VERSION 3.2)

project(ps4app)

option(CHIAKI_ENABLE_TESTS "Enable tests for Ps4App" ON)

add_subdirectory(lib)

if(CHIAKI_ENABLE_TESTS)
	enable_testing()
	add_subdirectory(test)
endif()