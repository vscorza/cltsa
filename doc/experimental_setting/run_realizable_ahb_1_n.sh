#!/bin/bash
DST_DIR=$1
OUTPUT_DIR=$2
CLTSA_DIR=$3
# 3
MAX_INSTANCE=$4
ITERATIONS=$5
INSTANCES=""

for ((i=1; i<=$MAX_INSTANCE;i++)) 
	do 
		i2=$((i+1))
		for((j=1; j<=$ITERATIONS;j++))do INSTANCES="${INSTANCES} ${DST_DIR}ahb_arbiter_${i2}_sched.fsp AHB.Scheduler.${i2}"; done
	done

cd ${CLTSA_DIR}
nice -20 ./cltsa -r -o $OUTPUT_DIR $INSTANCES

