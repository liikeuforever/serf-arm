add_test([=[TestLZ77.CorrectnessTest]=]  /Users/xuzihang/GitProject/Serf/build/test/lz77_test [==[--gtest_filter=TestLZ77.CorrectnessTest]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[TestLZ77.CorrectnessTest]=]  PROPERTIES WORKING_DIRECTORY /Users/xuzihang/GitProject/Serf/build/test SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set(  lz77_test_TESTS TestLZ77.CorrectnessTest)
