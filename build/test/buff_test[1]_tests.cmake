add_test([=[TestBuff.CorrectnessTest]=]  /Users/xuzihang/GitProject/Serf/build/test/buff_test [==[--gtest_filter=TestBuff.CorrectnessTest]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[TestBuff.CorrectnessTest]=]  PROPERTIES WORKING_DIRECTORY /Users/xuzihang/GitProject/Serf/build/test SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set(  buff_test_TESTS TestBuff.CorrectnessTest)
