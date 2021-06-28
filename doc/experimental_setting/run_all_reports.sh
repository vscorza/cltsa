#!/bin/bash
echo "[[Running all reports]]"
# "/home/mariano/code/henos-automata/src/tests/"
DST_DIR=$1
# "/home/mariano/code/henos-automata/doc/experimental_setting/tmp_results/"
OUTPUT_DIR=$2
# "../../src/"
CLTSA_DIR=$3
MAX_ITERATIONS=$4
TABLE_TYPE=$5
./run_realizable_full.sh $DST_DIR $OUTPUT_DIR $CLTSA_DIR $MAX_ITERATIONS
./run_unrealizable_full.sh $DST_DIR $OUTPUT_DIR $CLTSA_DIR $MAX_ITERATIONS
./run_seq_tests.sh $OUTPUT_DIR $CLTSA_DIR
./run_merge_files.sh $OUTPUT_DIR
./run_generate_latex_tables.sh $TABLE_TYPE
