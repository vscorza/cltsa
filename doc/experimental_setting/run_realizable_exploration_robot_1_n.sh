#!/bin/bash
MAX_INSTANCE=9
ITERATIONS=1
DST_DIR="/home/mariano/code/henos-automata/src/tests/"
OUTPUT_DIR="/home/mariano/code/henos-automata/doc/experimental_setting/tmp_results/"
CLTSA_DIR="../../src/"
INSTANCES=""

for ((i=1; i<=$MAX_INSTANCE;i++)) 
	do 
		i2=$((3+i*3))
		for((j=1; j<=$ITERATIONS;j++))do INSTANCES="${INSTANCES} ${DST_DIR}exploration-robot-v1_${i2}.fsp Exploration.Robot.${i2}"; done
	done

echo "Running missing_assumption exploration robot instances from size 1 to ${MAX_INSTANCE}"

cd ${CLTSA_DIR}
nice -20 ./cltsa -r -o $OUTPUT_DIR $INSTANCES

