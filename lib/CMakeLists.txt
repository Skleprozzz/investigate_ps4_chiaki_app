set(HEADER_FILES
        include/ps4app/session.h
        include/ps4app/common.h
        include/ps4app/thread.h
        include/ps4app/base64.h
        include/ps4app/http.h
        include/ps4app/log.h
        include/ps4app/ctrl.h
        include/ps4app/rpcrypt.h)

set(SOURCE_FILES
        src/common.c
        src/session.c
        src/thread.c
        src/base64.c
        src/http.c
        src/log.c
        src/ctrl.c
        src/rpcrypt.c)

add_library(ps4app-lib ${HEADER_FILES} ${SOURCE_FILES})
set_target_properties(ps4app-lib PROPERTIES OUTPUT_NAME ps4app)

target_include_directories(ps4app-lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

find_package(Threads)
target_link_libraries(ps4app-lib Threads::Threads)