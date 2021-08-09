#!/bin/bash
DST_DIR=$1
OUTPUT_DIR=$2
CLTSA_DIR=$3

cd ${CLTSA_DIR}
fileNames=("CM_K" "AT_N_K" "BW_N_K" "TL_N_K" "TA_N_K" "DP_N_K")
caseNames=("cat.mouse.K" "airport.tower.N.K" "bidding.workflow.N.K" "transfer.line.N.K" "travel.agency.N.K" "dining.philosophers.N.K")
		X CM Aut: 1,2 areas: 1 > 6
		X AT Aut:2,3,4,5 steps:1,2 > 8
		X BW Aut:1,2,3,4 steps:1,2 > 8
		X TL Aut: 2, 3, 4 steps 1, 2	> 6
		X TA Aut: 2, 3, 4 steps: 1, 2 > 6
		X DP Aut: 2, 3 steps: 1,2,3 > 6


fileNLow=("1" "2" "1" "2" "2" "2")
fileNHi=("1" "6" "5" "4" "5" "3")
fileKLow=("1" "1" "1" "1" "1" "1")
fileKHi=("2" "3" "1" "2" "3" "4")
fileSuffixes=(".fsp" "_missing_assumption.fsp" "_removed_safety.fsp")
caseSuffixes=(".realizable" ".missing.assumption" ".removed.safety")
	
INSTANCES=("" "" "")
	
for i in "${!fileNames[@]}"; do
	for j in "${!fileSuffixes[@]}"; do
		for (( n = ${fileNLow[i]} ; n <= ${fileNHi[i]} ; n++ )) 
		do
			for (( k="${fileKLow[i]}"; k<="${fileKHi[i]}"; k++ )) 
			do
				msg="${fileNames[i]/_N/_$n}"
				msg="${msg/_K/_$k}"
				caseName="${caseNames[i]/N/$n}"
				caseName="${caseName/K/$k}"				
				msg="${msg}${fileSuffixes[j]}"
				#echo "Copying ${msg} from ${fileNames[i]}${fileSuffixes[j]}  "
				cp "tests/${fileNames[i]}${fileSuffixes[j]}" "tests/${msg}"
				sed -i "s/\${N}/$n/" "tests/${msg}"
				sed -i "s/\${K}/$k/" "tests/${msg}"				
				INSTANCES[j]="${INSTANCES[j]} ${DST_DIR}${msg} ${caseName}${caseSuffixes[j]}"	
			done
		done		
	done
done	

for j in "${!fileSuffixes[@]}"; do
	echo "Running ${fileSuffixes[j]} DCS suffixed cases "			
	nice -20 ./cltsa -r -o $OUTPUT_DIR ${INSTANCES[j]}
done
#nice -20 ./cltsa -r -o $OUTPUT_DIR $INSTANCES
