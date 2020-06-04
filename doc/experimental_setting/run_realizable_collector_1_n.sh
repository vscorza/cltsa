MAX_INSTANCE=5
ITERATIONS=1
DST_DIR="/home/mariano/code/henos-automata/src/tests/"
CLTSA_DIR="../../src/"
INSTANCES=""

for ((i=1; i<=$MAX_INSTANCE;i++)) 
	do 
		for((j=1; j<=$ITERATIONS;j++))do INSTANCES="${INSTANCES} ${DST_DIR}collector_${i}_in_v1.fsp"; done
	done

echo "Running realizable colletor instances from size 1 to ${MAX_INSTANCE}"
echo "INSTANCES: ${INSTANCES}c"

cd ${CLTSA_DIR}
nice -20 ./cltsa -r $INSTANCES

