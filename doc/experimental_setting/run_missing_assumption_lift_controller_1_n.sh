#!/bin/bash
MAX_INSTANCE=4
ITERATIONS=(3 2 1 1)
DST_DIR="/home/mariano/code/henos-automata/src/tests/"
OUTPUT_DIR="/home/mariano/code/henos-automata/doc/experimental_setting/tmp_results/"
CLTSA_DIR="../../src/"
INSTANCES=""

echo "::[Running added goal lift controller instances from size 1 to ${MAX_INSTANCE}]::"
cd ${CLTSA_DIR}

for ((i=1; i<=$MAX_INSTANCE;i++)) 
	do 
		for((j=1; j<=${ITERATIONS[i-1]};j++))
			do 
				i2=$((i*2))
				INSTANCES="${INSTANCES} ${DST_DIR}lift_controller_${i2}_missing_assumption.fsp Lift.Controller.${i2}.(missing.assumption)"; 
				nice -20 ./cltsa -r -o $OUTPUT_DIR $INSTANCES
			done
	done
