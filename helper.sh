#!/bin/bash

if [[ $1 == "build" ]]; then
	cmake -DCMAKE_BUILD_TYPE=$2 -B build && cmake --build build
elif [[ $1 == "benchmark" ]]; then
	printf "%s\n" "[WIP] Run benchmark"
elif [[ $1 == "test" ]]; then
	./build/bin/serf_test
elif [[ $1 == "clean" ]]; then
	if [[ -e build ]]; then
		rm -r ./build
	fi
else
	printf "%s\n" "Serf Build Helper for everyone"
	printf "%s\n" "Usage: "
	printf "%s\n" "    helper.sh build [release/debug] - create build dir and build serf."
	printf "%s\n" "    helper.sh benchmark - run benchmark."
	printf "%s\n" "    helper.sh test - run all unit test."
	printf "%s\n" "    helper.sh clean - remove build dir and clean all things."
fi
