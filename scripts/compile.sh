#!/usr/bin/env bash

set -euxo pipefail

# See https://github.com/souffle-lang/souffle/issues/2069 about openmp
g++ -std=c++17 "$1" -fopenmp -c -fPIC -o functors.o
g++ -shared -o "$2" functors.o
