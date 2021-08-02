#!/bin/bash
DST_DIR=$1
OUTPUT_DIR=$2
CLTSA_DIR=$3

cd ${CLTSA_DIR}
fileNames=("CM_K" "AT_N_K" "BW_N_K" "TL_N_K" "TA_N_K" "DP_N_K")
caseNames=("cat.mouse.K" "airport.tower.N.K" "bidding.workflow.N.K" "transfer.line.N.K" "travel.agency.N.K" "dining.philosophers.N.K")
fileNLow=("1" "2" "1" "2" "2" "2")
fileNHi=("4" "5" "3" "3" "4" "4")
fileKLow=("1" "1" "1" "1" "1" "1")
fileKHi=("2" "2" "1" "3" "2" "2")
fileSuffixes=(".fsp" "_missing_assumption.fsp" "_removed_safety.fsp")
	
INSTANCES=""
	
for i in "${!fileNames[@]}"; do
	for fileSuffix in "${fileSuffixes[@]}"; do
		for (( n = ${fileNLow[i]} ; n <= ${fileNHi[i]} ; n++ )) 
		do
			for (( k="${fileKLow[i]}"; k<="${fileKHi[i]}"; k++ )) 
			do
				msg="${fileNames[i]/_N/_$n}"
				msg="${msg/_K/_$k}"
				caseName="${casesNames[i]/_N/_$n}"
				caseName="${caseName/_K/_$k}"				
				msg="${msg}${fileSuffix}"
				echo "Copying ${msg} from ${fileNames[i]}${fileSuffix}  "
				cp "tests/${fileNames[i]}${fileSuffix}" "tests/${msg}"
				sed -i "s/\${N}/$n/" "tests/${msg}"
				sed -i "s/\${K}/$k/" "tests/${msg}"				
				INSTANCES="${INSTANCES} ${DST_DIR}${msg} ${caseName}"				
			done
		done		
	done
done	

nice -20 ./cltsa -r -o $OUTPUT_DIR $INSTANCES
