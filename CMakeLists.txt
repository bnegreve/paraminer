
cmake_minimum_required(VERSION 2.6)
add_definitions()
include_directories(../../melinda)

add_executable(clogen_itemsets clogen.cpp element.cpp pattern.cpp utils.cpp)
target_link_libraries(pthread)

add_custom_target(clogen_doc doxygen doc)
