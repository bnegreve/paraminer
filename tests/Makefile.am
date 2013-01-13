TESTS_ENVIRONMENT = source $(builddir)/tests-env.sh;

testsdir = $(bindir)/tests

all_tests= test_fim_simple.sh \
	test_fim_simple_mt.sh \
	test_fim_full_compare.sh \
	test_fim_full_compare_mt.sh \
	test_crg_simple.sh \
	test_gri_full_compare.sh

dist_tests_SCRIPTS= $(all_tests)

TESTS = $(all_tests)