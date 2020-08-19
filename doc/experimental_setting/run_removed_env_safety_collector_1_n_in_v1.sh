#!/bin/bash
# "/home/mariano/code/henos-automata/src/tests/"
DST_DIR=$1
# "/home/mariano/code/henos-automata/doc/experimental_setting/tmp_results/"
OUTPUT_DIR=$2
# "../../src/"
CLTSA_DIR=$3
# 5
MAX_INSTANCE=$4
# (7 7 7 7 7)
ITERATIONS=$5
INSTANCES=""

echo "::[Running removed safety collector instances from size 1 to ${MAX_INSTANCE}]::"
cd ${CLTSA_DIR}

for ((i=1; i<=$MAX_INSTANCE;i++)) 
	do 
		for((j=1; j<=$ITERATIONS;j++))
			do 
				INSTANCES="${INSTANCES} ${DST_DIR}collector_${i}_in_v1_removed_safety.fsp Collector.${i}.(removed.safety)"; 
				nice -20 ./cltsa -r -o $OUTPUT_DIR $INSTANCES
			done
	done




