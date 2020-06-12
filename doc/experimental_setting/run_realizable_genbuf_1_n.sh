#!/bin/bash
MAX_INSTANCE=3
ITERATIONS=1
DST_DIR="/home/mariano/code/henos-automata/src/tests/"
OUTPUT_DIR="/home/mariano/code/henos-automata/doc/experimental_setting/tmp_results/"
CLTSA_DIR="../../src/"
INSTANCES=""

for ((i=1; i<=$MAX_INSTANCE;i++)) 
	do 
		for((j=1; j<=$ITERATIONS;j++))do INSTANCES="${INSTANCES} ${DST_DIR}genbuf_${i}_sndrs_no_automaton.fsp Genbuf.${i}"; done
	done

echo "Running realizable genbuf instances from size 1 to ${MAX_INSTANCE}"

cd ${CLTSA_DIR}
nice -20 ./cltsa -r -o $OUTPUT_DIR $INSTANCES

