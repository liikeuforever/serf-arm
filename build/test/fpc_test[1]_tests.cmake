add_test([=[TestFPC.CorrectnessTest]=]  /Users/xuzihang/GitProject/Serf/build/test/fpc_test [==[--gtest_filter=TestFPC.CorrectnessTest]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[TestFPC.CorrectnessTest]=]  PROPERTIES WORKING_DIRECTORY /Users/xuzihang/GitProject/Serf/build/test SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set(  fpc_test_TESTS TestFPC.CorrectnessTest)
