cmake_minimum_required(VERSION 2.6)

cmake_policy(SET CMP0015 NEW)
add_definitions(-DTEST -g)

set(SOURCES ../itemset_specific.cpp ../utils.cpp ../element.cpp ../pattern.cpp ../clogen.cpp)

include_directories("../")
link_directories("../")

add_executable(is_included_1d  is_included_1d.cpp ${SOURCES})
add_executable(transpose transpose.cpp ${SOURCES})

add_test(is_included_1d is_included_1d 1000)
add_test(transpose transpose)

