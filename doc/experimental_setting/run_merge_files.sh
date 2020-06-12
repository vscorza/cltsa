#!/bin/bash
echo "[[Merging files]]"
OUTPUT_DIR="/home/mariano/code/henos-automata/doc/experimental_setting/tmp_results/"
cd $OUTPUT_DIR
echo 'name,realizable,ltl_model_build_time,model_build_time,composition_time,synthesis_time,diagnosis_time,\
alphabet_size,guarantees_count,assumptions_count,plant_states,plant_transitions,plant_controllable_transitions,\
minimization_states,minimization_transitions,minimization_controllable_transitions,search_method,diagnosis_steps\n' > unrealizable_data_composite.csv
echo 'name,realizable,ltl_model_build_time,model_build_time,composition_time,synthesis_time,diagnosis_time,\
alphabet_size,guarantees_count,assumptions_count,plant_states,plant_transitions,plant_controllable_transitions,\
minimization_states,minimization_transitions,minimization_controllable_transitions,search_method,diagnosis_steps\n' > realizable_data_composite.csv
tail -q -n +2 collector_1_in_v1_missing_assumption.fsp.csv exploration-robot-v1_6_missing_assumption.fsp.csv genbuf_1_sndrs_no_automaton_missing_assumption.fsp.csv genbuf_1_sndrs_no_automaton_removed_env_safety.fsp.csv >> unrealizable_data_composite.csv
tail -q -n +2 collector_1_in_v1.fsp.csv exploration-robot-v1_6.fsp.csv genbuf_1_sndrs_no_automaton.fsp.csv >> realizable_data_composite.csv

