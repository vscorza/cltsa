#!/bin/bash
DST_DIR=$1
OUTPUT_DIR=$2
CLTSA_DIR=$3
# 2
MAX_INSTANCE=$4
ITERATIONS=$5

INSTANCES=""

echo "::[Running removed env safety genbuf instances from size 1 to ${MAX_INSTANCE}]::"
cd ${CLTSA_DIR}

for ((i=1; i<=$MAX_INSTANCE;i++)) 
	do 
		for((j=1; j<=$ITERATIONS;j++))
			do 
				INSTANCES="${INSTANCES} ${DST_DIR}genbuf_${i}_sndrs_no_automaton_removed_env_safety.fsp Genbuf.${i}.(removed.safety)"; 
			done
	done
	
nice -20 ./cltsa -r -o $OUTPUT_DIR $INSTANCES
