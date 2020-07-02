#!/bin/bash
DST_DIR=$1
OUTPUT_DIR=$2
CLTSA_DIR=$3
# 4
MAX_INSTANCE=$4
ITERATIONS=$5
INSTANCES=""

for ((i=1; i<=$MAX_INSTANCE;i++)) 
	do 
		i2=$((i * 2))
		for((j=1; j<=$ITERATIONS;j++))do INSTANCES="${INSTANCES} ${DST_DIR}lift_controller_${i2}.fsp Lift.Controller.${i2}"; done
	done

echo "Running realizable lift controller instances from size 1 to ${MAX_INSTANCE}"
echo "Will run nice -20 ./cltsa -r -o ${OUTPUT_DIR} ${INSTANCES}"
cd ${CLTSA_DIR}
nice -20 ./cltsa -r -o $OUTPUT_DIR $INSTANCES

