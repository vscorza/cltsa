MAX_INSTANCE=9
ITERATIONS=3
DST_DIR="/home/mariano/code/henos-automata/src/tests/"
CLTSA_DIR="../../src/"
INSTANCES=""

for ((i=1; i<=$MAX_INSTANCE;i++)) 
	do 
		i2=$((3+i*3))
		for((j=1; j<=$ITERATIONS;j++))do INSTANCES="${INSTANCES} ${DST_DIR}exploration-robot-v1_${i2}.fsp"; done
	done

echo "Running missing_assumption exploration robot instances from size 1 to ${MAX_INSTANCE}"
echo "INSTANCES: ${INSTANCES}c"

cd ${CLTSA_DIR}
nice -20 ./cltsa -r $INSTANCES

