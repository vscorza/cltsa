MAX_INSTANCE=3
ITERATIONS=5
DST_DIR="/home/mariano/code/henos-automata/src/tests/"
CLTSA_DIR="../../src/"
INSTANCES=""

for ((i=1; i<=$MAX_INSTANCE;i++)) 
	do 
		for((j=1; j<=$ITERATIONS;j++))do INSTANCES="${INSTANCES} ${DST_DIR}genbuf_${i}_sndrs_no_automaton.fsp"; done
	done

echo "Running realizable genbuf instances from size 1 to ${MAX_INSTANCE}"
echo "INSTANCES: ${INSTANCES}"

cd ${CLTSA_DIR}
nice -20 ./cltsa -r $INSTANCES

