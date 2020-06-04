#!/bin/bash
MAX_INSTANCE=2
DST_DIR="/home/mariano/code/henos-automata/src/tests/"
CLTSA_DIR="../../src/"
INSTANCES=""

echo "::[Running removed env safety genbuf instances from size 1 to ${MAX_INSTANCE}]::"
cd ${CLTSA_DIR}

for ((i=1; i<=$MAX_INSTANCE;i++)) 
	do 
		INSTANCES="${INSTANCES} ${DST_DIR}genbuf_${i}_sndrs_no_automaton_removed_env_safety_diff.fsp"; 
	done
echo "nice -20 ./cltsa -r -0 ${OUTPUT_DIR} ${INSTANCES}"
nice -20 ./cltsa -r -o $OUTPUT_DIR $INSTANCES
