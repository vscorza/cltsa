#!/bin/bash
valgrind --tool=callgrind ./main
kcachegrind callgrind.out.*
