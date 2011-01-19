
cmake_minimum_required(VERSION 2.6)
add_definitions()


include_directories(../../melinda)

set(CLOGEN_SOURCE  clogen.cpp element.cpp database.cpp pattern.cpp utils.cpp)

add_executable(clogen_itemsets  clogen_local_itemsets.cpp ${CLOGEN_SOURCE})
add_executable(clogen_cgraph ${CLOGEN_SOURCE} clogen_local_cgraph.cpp)
add_executable(clogen_graduals ${CLOGEN_SOURCE} clogen_local_graduals.cpp BinaryMatrix.cpp CException.cpp TransactionSequence.cpp)

#add_executable(clogen_dag ${CLOGEN_SOURCE} clogen_local_dag.cpp)
#SET_TARGET_PROPERTIES(clogen_itemsets PROPERTIES LINK_FLAGS -pg)
target_link_libraries(clogen_itemsets pthread melinda)
target_link_libraries(clogen_cgraph pthread melinda)
target_link_libraries(clogen_graduals pthread melinda)
#target_link_libraries(clogen_dag pthread melinda)

enable_testing()
add_subdirectory(test)

add_custom_target(clogen_doc doxygen doc)
