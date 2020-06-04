#!/bin/bash
MAX_INSTANCE=3
DST_DIR="/home/mariano/code/henos-automata/src/tests/"
CLTSA_DIR="../../src/"
OUTPUT_DIR="/home/mariano/code/henos-automata/doc/experimental_setting/tmp_results/"
INSTANCES=""

echo "::[Running missing assumption genbuf instances from size 1 to ${MAX_INSTANCE}]::"
cd ${CLTSA_DIR}

for ((i=1; i<=$MAX_INSTANCE;i++)) 
	do 
		INSTANCES="${INSTANCES} ${DST_DIR}genbuf_${i}_sndrs_no_automaton_missing_assumption_diff.fsp"; 
	done
echo "nice -20 ./cltsa -r -0 ${OUTPUT_DIR} ${INSTANCES}"
nice -20 ./cltsa -r -o $OUTPUT_DIR $INSTANCES
