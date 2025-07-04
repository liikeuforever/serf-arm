add_test([=[TestElf.CorrectnessTest]=]  /Users/xuzihang/GitProject/Serf/build/test/elf_test [==[--gtest_filter=TestElf.CorrectnessTest]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[TestElf.CorrectnessTest]=]  PROPERTIES WORKING_DIRECTORY /Users/xuzihang/GitProject/Serf/build/test SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set(  elf_test_TESTS TestElf.CorrectnessTest)
