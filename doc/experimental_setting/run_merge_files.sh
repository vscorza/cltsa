#!/bin/bash
echo "[[Merging files]]"
OUTPUT_DIR=$1
cd $OUTPUT_DIR
echo 'name,realizable,ltl_model_build_time,model_build_time,composition_time,synthesis_time,diagnosis_time,alphabet_size,guarantees_count,assumptions_count,plant_states,plant_transitions,plant_controllable_transitions,plant_controllable_options,minimization_states,minimization_transitions,minimization_controllable_transitions,minimization_controllable_options,search_method,diagnosis_steps' > unrealizable_data_composite.csv
echo 'name,realizable,ltl_model_build_time,model_build_time,composition_time,synthesis_time,diagnosis_time,alphabet_size,guarantees_count,assumptions_count,plant_states,plant_transitions,plant_controllable_transitions,plant_controllable_options,minimization_states,minimization_transitions,minimization_controllable_transitions,minimization_controllable_options,search_method,diagnosis_steps' > realizable_data_composite.csv
echo 'filename, automaton_name, states_count, transitions_count, mean_delta_s, variance_delta_s, total_signals, mean_signals_t, variance_signals_t, controllable_options, mean_controllable_options, variance_controllable_options' > seq_data_composite.csv
tail -q -n +2 lift_controller_2_missing_assumption.fsp.csv lift_controller_2_removed_safety.fsp.csv collector_1_in_v1_missing_assumption.fsp.csv collector_1_in_v1_removed_safety.fsp.csv exploration-robot-v1_100_missing_assumption.fsp.csv exploration-robot-v1_100_removed_safety.fsp.csv ahb_arbiter_2_sched_missing_assumption.fsp.csv ahb_arbiter_2_sched_removed_safety.fsp.csv genbuf_1_sndrs_no_automaton_missing_assumption.fsp.csv genbuf_1_sndrs_no_automaton_removed_env_safety.fsp.csv >> unrealizable_data_composite.csv
tail -q -n +2 lift_controller_2.fsp.csv  collector_1_in_v1.fsp.csv exploration-robot-v1_100.fsp.csv ahb_arbiter_2_sched.fsp.csv genbuf_1_sndrs_no_automaton.fsp.csv >> realizable_data_composite.csv
head -n1 -q /tmp/*.diag >> seq_data_composite.csv


