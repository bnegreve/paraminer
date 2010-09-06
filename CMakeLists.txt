
cmake_minimum_required(VERSION 2.6)
add_definitions(-pg)
include_directories(../../melinda)

add_executable(clogen_itemsets clogen.cpp element.cpp pattern.cpp utils.cpp itemset_specific.cpp)
target_link_libraries(pthread)

enable_testing()
add_subdirectory(test)

add_custom_target(clogen_doc doxygen doc)
