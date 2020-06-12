#!/bin/bash
MAX_INSTANCE=5
ITERATIONS=1
DST_DIR="/home/mariano/code/henos-automata/src/tests/"
OUTPUT_DIR="/home/mariano/code/henos-automata/doc/experimental_setting/tmp_results/"
CLTSA_DIR="../../src/"
INSTANCES=""

for ((i=1; i<=$MAX_INSTANCE;i++)) 
	do 
		for((j=1; j<=$ITERATIONS;j++))do INSTANCES="${INSTANCES} ${DST_DIR}collector_${i}_in_v1.fsp Collector.${1}"; done
	done

echo "Running realizable colletor instances from size 1 to ${MAX_INSTANCE}"

cd ${CLTSA_DIR}
nice -20 ./cltsa -r -o $OUTPUT_DIR $INSTANCES

