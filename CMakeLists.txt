cmake_minimum_required(VERSION 2.8)

project(FrequentSnapshot)
set(CMAKE_C_STANDARD 99)

set(ADDRESS_MODEL 64)
set(NODE_TARGET x64)

#add_definitions(-D VERBOSE)

add_definitions(-D_GNU_SOURCE)
add_definitions(-D TICK_UPDATE)
add_definitions(-D VERBOSE)
add_definitions(-D TICK_UPDATE)

link_libraries(m)
link_libraries(numa)
link_libraries(pthread)


include_directories(.)

add_executable(app src/system/app.c src/system/system.c src/system/util.c src/alg/cou.c src/alg/LL.c src/alg/myfork.c src/alg/mk.c src/alg/naive.c src/alg/pingpong.c src/alg/zigzag.c)
