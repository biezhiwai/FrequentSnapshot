cmake_minimum_required(VERSION 2.8)

project(FrequentSnapshot)

# set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(ADDRESS_MODEL 64)
set(NODE_TARGET x64)
set(CMAKE_C_FLAGS "$ENV{CFLAGS} -m64  -O2 -fprefetch-loop-arrays -Wall")

add_definitions(-D_GNU_SOURCE)
add_definitions(-D TICK_UPDATE)
add_definitions(-D VERBOSE)


link_libraries(m)
link_libraries(numa)
link_libraries(pthread)


include_directories(.)

add_executable(app src/alg/main.c src/alg/system.c src/alg/util.c src/alg/cou.c src/alg/HG.c src/alg/myfork.c src/alg/PB.c src/alg/naive.c src/alg/pingpong.c src/alg/zigzag.c src/alg/myfork_hotcold.c src/alg/myfork_hotcold_cou.c src/alg/myfork_lru.c)
add_executable(HugePageOnFork src/alg/HugePageOnFork.c src/alg/util.c)