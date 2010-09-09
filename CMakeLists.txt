
cmake_minimum_required(VERSION 2.6)
add_definitions(-g -DNDEBUG -O3)
include_directories(../../melinda)

add_executable(clogen_itemsets clogen.cpp element.cpp pattern.cpp utils.cpp itemset_specific.cpp)
target_link_libraries(clogen_itemsets pthread melinda)

enable_testing()
#add_subdirectory(test)

add_custom_target(clogen_doc doxygen doc)
