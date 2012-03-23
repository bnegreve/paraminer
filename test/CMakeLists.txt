cmake_minimum_required(VERSION 2.6)

#cmake_policy(SET CMP0015 NEW)
add_definitions(-DTEST -g)

set(SOURCES ../itemset_specific.cpp ../utils.cpp ../element.cpp ../pattern.cpp ../clogen.cpp)

include_directories("../")
link_directories("../")

#add_executable(is_included_1d  is_included_1d.cpp ${SOURCES})
#add_executable(transpose transpose.cpp ${SOURCES})

add_custom_target(frequent_itemset_basic.sh DEPENDS clogen_itemsets)

add_test(cgraph_simple compare_outputs.sh expected_output/cgraph_simple.dat \()
add_test(cgraph_hughes compare_outputs.sh expected_output/cgraph_hughes.dat \()
add_test(cgraph_hughes_gspan cgraph_gspan_test.sh 0.55)
add_test(frequent_itemset_mushroom frequent_itemset_basic.sh ../data/itemsets/mushroom.dat 4000)
add_test(frequent_itemset_mt_mushroom  frequent_itemset_basic.sh ../data/itemsets/mushroom.dat 5000 4)
add_test(frequent_itemset_full_compare full_compare.sh ../data/itemsets/mushroom.dat 2500 1)


add_test(graduals_glcm_full grad_glcm_full.sh 0.10)
add_test(graduals_test1@50 compare_outputs.sh expected_output/grad_test1_50 +)
add_test(graduals_test_equal@1 compare_outputs.sh expected_output/grad_test_equal@1 +)
add_test(graduals_test_merge@1 compare_outputs.sh expected_output/grad_test_merge@1 +)
add_test(graduals_neq_test1@50 compare_outputs.sh expected_output/clogen_grad_neq +)
#this test is to be validated 


#add_test(transpose transpose)

