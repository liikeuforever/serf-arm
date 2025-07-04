add_test([=[TestLZ4.CorrectnessTest]=]  /Users/xuzihang/GitProject/Serf/build/test/lz4_test [==[--gtest_filter=TestLZ4.CorrectnessTest]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[TestLZ4.CorrectnessTest]=]  PROPERTIES WORKING_DIRECTORY /Users/xuzihang/GitProject/Serf/build/test SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set(  lz4_test_TESTS TestLZ4.CorrectnessTest)
