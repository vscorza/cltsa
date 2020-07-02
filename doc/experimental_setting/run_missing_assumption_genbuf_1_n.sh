#!/bin/bash
DST_DIR=$1
OUTPUT_DIR=$2
CLTSA_DIR=$3
# 3
MAX_INSTANCE=$4
ITERATIONS=$5
INSTANCES=""

echo "::[Running missing assumption genbuf instances from size 1 to ${MAX_INSTANCE}]::"
cd ${CLTSA_DIR}

for ((i=1; i<=$MAX_INSTANCE;i++)) 
	do 
		for((j=1; j<=$ITERATIONS;j++))
			do 
				INSTANCES="${INSTANCES} ${DST_DIR}genbuf_${i}_sndrs_no_automaton_missing_assumption.fsp Genbuf.${i}.(missing.assumption)"; 
			done
	done

nice -20 ./cltsa -r -o $OUTPUT_DIR $INSTANCES
