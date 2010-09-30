
cmake_minimum_required(VERSION 2.6)
add_definitions()


include_directories(../../melinda)

add_executable(clogen_itemsets clogen.cpp element.cpp pattern.cpp utils.cpp itemset_specific.cpp)
#SET_TARGET_PROPERTIES(clogen_itemsets PROPERTIES LINK_FLAGS -pg)
target_link_libraries(clogen_itemsets pthread melinda)

enable_testing()
#add_subdirectory(test)

add_custom_target(clogen_doc doxygen doc)
