#!/bin/bash
MAX_INSTANCE=9
ITERATIONS=1
DST_DIR="/home/mariano/code/henos-automata/src/tests/"
CLTSA_DIR="../../src/"
OUTPUT_DIR="/home/mariano/code/henos-automata/doc/experimental_setting/tmp_results/"
INSTANCES=""

echo "::[Running missing assumption exploration robot instances from size 6 to ${((MAX_INSTANCE*3))}]::"
cd ${CLTSA_DIR}



for ((i=1; i<=$MAX_INSTANCE;i++)) 
	do 
		i2=$((3+i*3))
		for((j=1; j<=$ITERATIONS;j++))do INSTANCES="${INSTANCES} ${DST_DIR}exploration_robot_${i2}_missing_assumption_diff.fsp"; done
	done
echo "nice -20 ./cltsa -r -o ${OUTPUT_DIR} ${INSTANCES}"
nice -20 ./cltsa -r -o $OUTPUT_DIR $INSTANCES
