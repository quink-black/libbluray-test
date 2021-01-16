cmake_minimum_required(VERSION 3.17)
project(bluray)

set(CMAKE_CXX_STANDARD 14)

find_package(PkgConfig)

pkg_check_modules(LIBBLURAY REQUIRED libbluray)

add_executable(bluray main.cpp bluraydump.h)

target_include_directories(bluray PUBLIC
        ${LIBBLURAY_INCLUDE_DIRS})

target_link_libraries(bluray PRIVATE
        ${LIBBLURAY_LDFLAGS})