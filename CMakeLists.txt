cmake_minimum_required(VERSION 3.10.2)
set(CMAKE_CXX_STANDARD 20)
project(cppquiz VERSION 0.1 LANGUAGES CXX)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set(CLANGFORMAT_EXECUTABLE clang-format-12)
set(Boost_USE_STATIC_LIBS ON)
include(ClangFormat)
find_package(Threads)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold")
else()
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,notext -Wno-unknown-pragmas")
endif()

if (DEBUG)
    message("This is a DEBUG build.")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        add_definitions("-std=c++20 -O0 -g -stdlib=libstdc++ -fno-omit-frame-pointer -Wall -mssse3")
    else()
        add_definitions("-std=c++2a -O0 -g                   -fno-omit-frame-pointer -Wall -mssse3 -Wno-unknown-pragmas -Wno-literal-suffix")
    endif()
else()
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        add_definitions("-std=c++20 -O2 -funsigned-char -stdlib=libstdc++ -Wall -fno-omit-frame-pointer -pedantic -pthread -mssse3 -Wno-reserved-user-defined-literal")
    else()
        add_definitions("-std=c++2a -O2 -funsigned-char                   -Wall -fno-omit-frame-pointer -pedantic -pthread -mssse3 -Wno-reserved-user-defined-literal -Wno-unknown-pragmas -Wno-literal-suffix")
    endif()
endif()

include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/"
    "/opt/cppse/build/seasocks/include/"
    "/opt/cppse/build/fmt/include/"
)

file(GLOB_RECURSE cppquiz_SOURCES "src/**.cpp" "src/**.hpp" "src/**.h")

add_executable(cppquiz ${cppquiz_SOURCES})

target_link_libraries(cppquiz PRIVATE -static-libgcc -static-libstdc++)
target_link_libraries(cppquiz PRIVATE /opt/cppse/build/seasocks/lib/libseasocks.a)
target_link_libraries(cppquiz PRIVATE /usr/lib/x86_64-linux-gnu/libz.a)

install (TARGETS cppquiz DESTINATION bin)

set(all_sources ${cppquiz_SOURCES})
list(REMOVE_DUPLICATES all_sources)
clangformat_setup(${all_sources})