add_test([=[TestElfStar.CorrectnessTest]=]  /Users/xuzihang/GitProject/Serf/build/test/elf_star_test [==[--gtest_filter=TestElfStar.CorrectnessTest]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[TestElfStar.CorrectnessTest]=]  PROPERTIES WORKING_DIRECTORY /Users/xuzihang/GitProject/Serf/build/test SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set(  elf_star_test_TESTS TestElfStar.CorrectnessTest)
