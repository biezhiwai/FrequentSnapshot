cmake_minimum_required(VERSION 2.8)

project(FrequentSnapshot)

link_libraries(m)
link_libraries(numa)
link_libraries(pthread)

add_definitions(-D _FILE_OFFSET_BITS=64 )
add_definitions(-D TICK_UPDATE)


if(CMAKE_CL_64)    #CMAKE的内建变量，如果是true，就说明编译器的64位的，自然可以编译64bit的程序
    set(ADDRESS_MODEL 64)
    set(NODE_TARGET x64)
else()
    set(ADDRESS_MODEL 32)
    set(NODE_TARGET x86)
endif()

include_directories(.)

add_executable(app sys/app.c sys/system.c sys/util.c alg/cou.c alg/LL.c alg/myfork.c alg/mk.c alg/naive.c alg/pingpong.c alg/zigzag.c)