cmake_minimum_required(VERSION 2.8)

project(FrequentSnapshot)


set(ADDRESS_MODEL 64)
set(NODE_TARGET x64)

#add_definitions(-D VERBOSE)

add_definitions(-D_GNU_SOURCE)
add_definitions(-D TICK_UPDATE)

add_definitions(-D TICK_UPDATE)

link_libraries(m)
link_libraries(numa)
link_libraries(pthread)


include_directories(.)

add_executable(app system/app.c system/system.c system/util.c alg/cou.c alg/LL.c alg/myfork.c alg/mk.c alg/naive.c alg/pingpong.c alg/zigzag.c)