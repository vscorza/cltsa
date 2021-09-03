#!/bin/bash
#!/bin/bash
STARTTIME=$(date +%s)
echo "[[Running all reports for CONFNAME]]"
echo "[[Building the tool]]"
make clean
make 2> /dev/null
rm -f *.o


############ REALIZABLE CASES 
REAL_STARTTIME=$(date +%s)
echo "[[Running realizable examples]]"
echo "[Lift controller]"
INSTANCES=""
for ((i=1; i<=4;i++)) 
	do 
		i2=$((i * 2))
		INSTANCES="${INSTANCES} tests/lift_controller_${i2}.fsp Lift.Controller.${i2}";
	done

./artifact -r -o results/ $INSTANCES
echo "[Collector]"
INSTANCES=""
for ((i=1; i<=5;i++)) 
	do 
		INSTANCES="${INSTANCES} tests/collector_${i}_in_v1.fsp Collector.${i}";
	done

./artifact -r -o results/ $INSTANCES
echo "[Exploration Robot]"
INSTANCES=""
for ((i=1; i<=6;i++)) 
	do 
		i2=$((i*100))
		INSTANCES="${INSTANCES} tests/exploration-robot-v1_${i2}.fsp Exploration.Robot.${i2}";
	done
./artifact -r -o results/ $INSTANCES
echo "[GenBuf]"
INSTANCES=""
for ((i=1; i<=3;i++)) 
	do 
		INSTANCES="${INSTANCES} tests/genbuf_${i}_sndrs_no_automaton.fsp Genbuf.${i}";
	done
./artifact -r -o results/ $INSTANCES
echo "[AHB]"
INSTANCES=""
for ((i=1; i<=3;i++)) 
	do 
		i2=$((i+1))
		INSTANCES="${INSTANCES} tests/ahb_arbiter_${i2}_sched.fsp AHB.Scheduler.${i2}"; 
	done
./artifact -r -o results/ $INSTANCES
REAL_ENDTIME=$(date +%s)
echo "It took $(($REAL_ENDTIME - $REAL_STARTTIME)) seconds to run the realizable specifications."
############ UNREALIZABLE CASES 
UNREAL_STARTTIME=$(date +%s)
echo "[[Running unrealizable examples]]"
echo "[Lift controller]"
echo "::[Running added goal lift controller instances from size 1 to 4]::"
INSTANCES=""
for ((i=1; i<=4;i++)) 
	do 
		i2=$((i*2))
		INSTANCES="${INSTANCES} tests/lift_controller_${i2}_missing_assumption.fsp Lift.Controller.${i2}.(missing.liveness)"; 
	done
./artifact -r -o results/ $INSTANCES
echo "::[Running removed env safety lift controller instances from size 1 to 4]::"
INSTANCES=""
for ((i=1; i<=4;i++)) 
	do 
		i2=$((i*2))
		INSTANCES="${INSTANCES} tests/lift_controller_${i2}_removed_safety.fsp Lift.Controller.${i2}.(removed.safety)"; 
	done
./artifact -r -o results/ $INSTANCES
echo "[Collector]"
echo "::[Running missing assumption collector instances from size 1 to 5]::"
INSTANCES=""
for ((i=1; i<=5;i++)) 
	do 
		INSTANCES="${INSTANCES} tests/collector_${i}_in_v1_missing_assumption.fsp Collector.${i}.(missing.liveness)"; 
	done
./artifact -r -o results/ $INSTANCES
echo "::[Running removed safety collector instances from size 1 to 5]::"
INSTANCES=""
for ((i=1; i<=5;i++)) 
	do 
		INSTANCES="${INSTANCES} tests/collector_${i}_in_v1_removed_safety.fsp Collector.${i}.(removed.safety)"; 
	done
./artifact -r -o results/ $INSTANCES
echo "[Exploration Robot]"
echo "::[Running missing assumption exploration robot instances from size 1 to 6]::"
INSTANCES=""
for ((i=1; i<=6;i++)) 
	do 
		i2=$((i*100))
		INSTANCES="${INSTANCES} tests/exploration-robot-v1_${i2}_missing_assumption.fsp Exploration.Robot.${i2}.(missing.liveness)"; 
	done
./artifact -r -o results/ $INSTANCES	
echo "::[Running removed safety exploration robot instances from size 1 to 6]::"
INSTANCES=""
for ((i=1; i<=6;i++)) 
	do 
		i2=$((i*100))
		INSTANCES="${INSTANCES} tests/exploration-robot-v1_${i2}_removed_safety.fsp Exploration.Robot.${i2}.(removed.safety)"; 
	done
./artifact -r -o results/ $INSTANCES	
echo "[GenBuf]"
echo "::[Running missing assumption genbuf instances from size 1 to 3]::"
INSTANCES=""
for ((i=1; i<=3;i++)) 
	do 
		INSTANCES="${INSTANCES} tests/genbuf_${i}_sndrs_no_automaton_missing_assumption.fsp Genbuf.${i}.(missing.assumption)"; 
	done
./artifact -r -o results/ $INSTANCES	
echo "::[Running removed env safety genbuf instances from size 1 to 2]::"
INSTANCES=""
for ((i=1; i<=2;i++)) 
	do 
		INSTANCES="${INSTANCES} tests/genbuf_${i}_sndrs_no_automaton_removed_env_safety.fsp Genbuf.${i}.(removed.safety)"; 
	done
./artifact -r -o results/ $INSTANCES	
echo "[AHB]"
echo "::[Running missing assumption ahb instances from size 1 to 3]::"
INSTANCES=""
for ((i=1; i<=3;i++)) 
	do 
		i2=$((i+1))
		INSTANCES="${INSTANCES} tests/ahb_arbiter_${i2}_sched_missing_assumption.fsp AHB.Scheduler.${i2}.(missing.assumption)"; 
	done
./artifact -r -o results/ $INSTANCES	
echo "::[Running removed safety ahb instances from size 1 to 3]::"
INSTANCES=""
for ((i=1; i<=3;i++)) 
	do 
		i2=$((i+1))
		INSTANCES="${INSTANCES} tests/ahb_arbiter_${i2}_sched_removed_safety.fsp AHB.Scheduler.${i2}.(removed.safety)"; 
	done
./artifact -r -o results/ $INSTANCES	
UNREAL_ENDTIME=$(date +%s)
echo "It took $(($UNREAL_ENDTIME - $UNREAL_STARTTIME)) seconds  to run the unrealizable specifications."
############ RUN DCS CASES
DCS_STARTTIME=$(date +%s)
echo "[[Running DCS examples]]"
fileNames=("CM_K" "AT_N_K" "BW_N_K" "TL_N_K" "TA_N_K" "DP_N_K")
caseNames=("cat.mouse.K" "airport.tower.N.K" "bidding.workflow.N.K" "transfer.line.N.K" "travel.agency.N.K" "dining.philosophers.N.K")
fileNLow=("1" "2" "1" "2" "2" "2")
fileNHi=("1" "5" "3" "3" "4" "4")
fileKLow=("1" "1" "1" "1" "1" "1")
fileKHi=("2" "2" "1" "3" "2" "2")
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
				INSTANCES[j]="${INSTANCES[j]} tests/${msg} ${caseName}${caseSuffixes[j]}"	
			done
		done		
	done
done	

for j in "${!fileSuffixes[@]}"; do
	echo "[Running ${fileSuffixes[j]} DCS suffixed cases]"			
	./artifact -r -o results/ ${INSTANCES[j]}
done
DCS_ENDTIME=$(date +%s)
echo "It took $(($DCS_ENDTIME - $DCS_STARTTIME)) seconds to run the dcs specifications."
############ MERGING FILES
echo "[[Merging files]]"
cd results
echo 'name,realizable,ltl_model_build_time,model_build_time,composition_time,synthesis_time,diagnosis_time,alphabet_size,guarantees_count,assumptions_count,plant_states,plant_transitions,plant_controllable_transitions,plant_controllable_options,minimization_states,minimization_transitions,minimization_controllable_transitions,minimization_controllable_options,search_method,diagnosis_steps' > unrealizable_data_composite.csv
echo 'name,realizable,ltl_model_build_time,model_build_time,composition_time,synthesis_time,diagnosis_time,alphabet_size,guarantees_count,assumptions_count,plant_states,plant_transitions,plant_controllable_transitions,plant_controllable_options,minimization_states,minimization_transitions,minimization_controllable_transitions,minimization_controllable_options,search_method,diagnosis_steps' > realizable_data_composite.csv
tail -q -n +2 lift_controller_2_missing_assumption.fsp.csv lift_controller_2_removed_safety.fsp.csv collector_1_in_v1_missing_assumption.fsp.csv collector_1_in_v1_removed_safety.fsp.csv exploration-robot-v1_100_missing_assumption.fsp.csv exploration-robot-v1_100_removed_safety.fsp.csv ahb_arbiter_2_sched_missing_assumption.fsp.csv ahb_arbiter_2_sched_removed_safety.fsp.csv genbuf_1_sndrs_no_automaton_missing_assumption.fsp.csv genbuf_1_sndrs_no_automaton_removed_env_safety.fsp.csv CM_1_missing_assumption.fsp.csv CM_1_removed_safety.fsp.csv >> unrealizable_data_composite.csv
tail -q -n +2 lift_controller_2.fsp.csv lift_controller_4.fsp.csv lift_controller_6.fsp.csv lift_controller_8.fsp.csv lift_controller_10.fsp.csv collector_1_in_v1.fsp.csv collector_2_in_v1.fsp.csv collector_3_in_v1.fsp.csv collector_4_in_v1.fsp.csv collector_5_in_v1.fsp.csv collector_6_in_v1.fsp.csv exploration-robot-v1_100.fsp.csv exploration-robot-v1_200.fsp.csv exploration-robot-v1_300.fsp.csv exploration-robot-v1_400.fsp.csv exploration-robot-v1_500.fsp.csv exploration-robot-v1_600.fsp.csv ahb_arbiter_2_sched.fsp.csv ahb_arbiter_3_sched.fsp.csv ahb_arbiter_4_sched.fsp.csv genbuf_1_sndrs_no_automaton.fsp.csv genbuf_2_sndrs_no_automaton.fsp.csv genbuf_3_sndrs_no_automaton.fsp.csv CM_1.fsp.csv >> realizable_data_composite.csv
cd ..

STARTTIME=$(date +%s)

echo "It took $(($ENDTIME - $STARTTIME))seconds to run the whole process."


