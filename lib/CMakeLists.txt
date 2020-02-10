set(HEADER_FILES
        include/app/session.h
        include/app/common.h
        include/app/thread.h
        include/app/base64.h)

set(SOURCE_FILES
        src/session.c
        src/thread.c
        src/base64.c)

add_library(app-lib ${HEADER_FILES} ${SOURCE_FILES})
set_target_properties(app-lib PROPERTIES OUTPUT_NAME app)

target_include_directories(app-lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

find_package(Threads)
target_link_libraries(app-lib Threads::Threads)