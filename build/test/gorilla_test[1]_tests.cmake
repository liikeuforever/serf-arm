add_test([=[TestGorilla.CorrectnessTest]=]  /Users/xuzihang/GitProject/Serf/build/test/gorilla_test [==[--gtest_filter=TestGorilla.CorrectnessTest]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[TestGorilla.CorrectnessTest]=]  PROPERTIES WORKING_DIRECTORY /Users/xuzihang/GitProject/Serf/build/test SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set(  gorilla_test_TESTS TestGorilla.CorrectnessTest)
