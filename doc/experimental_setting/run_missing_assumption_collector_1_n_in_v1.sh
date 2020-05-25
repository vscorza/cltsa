#!/bin/bash
MAX_INSTANCE=5
ITERATIONS=(7 5 3 2 1)
DST_DIR="/home/mariano/code/henos-automata/src/tests/"
OUTPUT_DIR="/home/mariano/code/henos-automata/doc/experimental_setting/tmp_results/"
CLTSA_DIR="../../src/"
INSTANCES=""

echo "::[Running missing assumption collector instances from size 1 to ${MAX_INSTANCE}]::"
cd ${CLTSA_DIR}

for ((i=1; i<=$MAX_INSTANCE;i++)) 
	do 
		for((j=1; j<=${ITERATIONS[i-1]};j++))
			do 
				INSTANCES="${INSTANCES} ${DST_DIR}collector_${i}_in_v1_missing_assumption.fsp"; 
				nice -20 ./cltsa -r -o $OUTPUT_DIR $INSTANCES
			done
	done




