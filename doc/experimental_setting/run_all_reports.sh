#!/bin/bash
echo "[[Running all reports]]"
./run_realizable_full.sh
./run_unrealizable_full.sh
./run_merge_files.sh
./run_add_realizable_columns.sh
./run_generate_latex_tables.sh
