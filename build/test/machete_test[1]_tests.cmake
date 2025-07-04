add_test([=[TestMachete.CorrectnessTest]=]  /Users/xuzihang/GitProject/Serf/build/test/machete_test [==[--gtest_filter=TestMachete.CorrectnessTest]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[TestMachete.CorrectnessTest]=]  PROPERTIES WORKING_DIRECTORY /Users/xuzihang/GitProject/Serf/build/test SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set(  machete_test_TESTS TestMachete.CorrectnessTest)
