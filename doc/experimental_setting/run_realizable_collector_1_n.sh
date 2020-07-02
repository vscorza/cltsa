#!/bin/bash
DST_DIR=$1
OUTPUT_DIR=$2
CLTSA_DIR=$3
# 5
MAX_INSTANCE=$4
ITERATIONS=1
INSTANCES=""

for ((i=1; i<=$MAX_INSTANCE;i++)) 
	do 
		for((j=1; j<=$ITERATIONS;j++))do INSTANCES="${INSTANCES} ${DST_DIR}collector_${i}_in_v1.fsp Collector.${i}"; done
	done

echo "Running realizable colletor instances from size 1 to ${MAX_INSTANCE}"
echo "Will run nice -20 ./cltsa -r -o ${OUTPUT_DIR} ${INSTANCES}"
cd ${CLTSA_DIR}
nice -20 ./cltsa -r -o $OUTPUT_DIR $INSTANCES

