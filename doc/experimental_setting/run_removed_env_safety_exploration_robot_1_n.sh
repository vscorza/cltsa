#!/bin/bash
DST_DIR=$1
OUTPUT_DIR=$2
CLTSA_DIR=$3
#9
MAX_INSTANCE=$4
ITERATIONS=$5

INSTANCES=""

echo "::[Running removed safety exploration robot instances from size 1 to ${MAX_INSTANCE}]::"
cd ${CLTSA_DIR}

for ((i=1; i<=$MAX_INSTANCE;i++)) 
	do 
		for((j=1; j<=$ITERATIONS;j++))
			do 
				i2=$((i*100))
				INSTANCES="${INSTANCES} ${DST_DIR}exploration-robot-v1_${i2}_removed_safety.fsp Exploration.Robot.${i2}.(removed.safety)"; 
			done
	done

nice -20 ./cltsa -r -o $OUTPUT_DIR $INSTANCES
