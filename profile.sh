#!/bin/sh

rm xray-log.test.*
clang++ --stdlib=libc++ -fxray-instrument -fxray-instruction-threshold=1 -O3  -ggdb3 -o test test.cc tinyrulechecker.cc
XRAY_OPTIONS="patch_premain=true xray_mode=xray-basic verbosity=1" BENCHMARK_ITERATIONS=100000 ./test
llvm-xray convert --symbolize --instr_map=test --output-format=trace_event xray-log.test.* | gzip> test-trace.txt.gz

