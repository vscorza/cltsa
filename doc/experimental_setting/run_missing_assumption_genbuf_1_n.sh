#!/bin/bash
MAX_INSTANCE=3
ITERATIONS=(7 7 7)
DST_DIR="/home/mariano/code/henos-automata/src/tests/"
OUTPUT_DIR="/home/mariano/code/henos-automata/doc/experimental_setting/tmp_results/"
CLTSA_DIR="../../src/"
INSTANCES=""

echo "::[Running missing assumption genbuf instances from size 1 to ${MAX_INSTANCE}]::"
cd ${CLTSA_DIR}

for ((i=1; i<=$MAX_INSTANCE;i++)) 
	do 
		for((j=1; j<=${ITERATIONS[i-1]};j++))
			do 
				INSTANCES="${INSTANCES} ${DST_DIR}genbuf_${i}_sndrs_no_automaton_missing_assumption.fsp Genbuf.${i}.(missing.assumption)"; 
			done
	done

nice -20 ./cltsa -r -o $OUTPUT_DIR $INSTANCES
