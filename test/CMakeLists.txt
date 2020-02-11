add_executable(ps4app-test
        main.c)

target_link_libraries(ps4app-test ps4app-lib ws2_32)