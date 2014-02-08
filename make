#!/bin/bash

# just a laziness script - no biggie

# pass in 'debug' to get -g -pg -ggdb3 throughout
target=${1:-all}

make --no-print-directory clean $target test

