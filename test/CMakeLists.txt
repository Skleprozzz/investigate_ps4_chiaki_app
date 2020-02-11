
add_library(munit "${CMAKE_CURRENT_SOURCE_DIR}/munit/munit.c")
target_include_directories(munit PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/munit")

add_executable(ps4app-unit
		main.c
		http.c)



target_link_libraries(ps4app-unit ps4app-lib ws2_32)

add_test(unit chiaki-unit)