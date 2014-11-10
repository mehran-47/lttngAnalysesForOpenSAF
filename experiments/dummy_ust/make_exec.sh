#!/bin/bash
g++ tracef_test.cpp -o exec.o -ldl -llttng-ust
./exec.o