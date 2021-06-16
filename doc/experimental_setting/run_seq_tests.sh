#!/bin/bash
DST_DIR=$1
OUTPUT_DIR=$2
CLTSA_DIR=$3

echo "::[Running seq tests]::"
cd ${CLTSA_DIR}

nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/ahb_arbiter_2.fsp
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/ahb_arbiter_2_sched.fsp
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/ahb_arbiter_3_sched.fsp
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/ahb_arbiter_4_sched.fsp
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/ahb_arbiter_6_sched.fsp
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/collector_1_in_v1.fsp
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/collector_2_in_v1.fsp
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/collector_3_in_v1.fsp
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/collector_4_in_v1.fsp
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/collector_5_in_v1.fsp
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/collector_6_in_v1.fsp
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/exploration-robot-v1_100.fsp
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/exploration-robot-v1_10.fsp
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/exploration-robot-v1_150.fsp
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/exploration-robot-v1_200.fsp
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/exploration-robot-v1_20.fsp
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/exploration-robot-v1_250.fsp
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/exploration-robot-v1_30.fsp
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/exploration-robot-v1_40.fsp
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/exploration-robot-v1_50.fsp
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/genbuf_1_sndrs_no_automaton.fsp
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/genbuf_2_sndrs_no_automaton.fsp
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/genbuf_3_sndrs_no_automaton.fsp
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/genbuf_4_sndrs_no_automaton.fsp
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/lift_controller_10.fsp
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/lift_controller_2.fsp
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/lift_controller_4.fsp
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/lift_controller_6.fsp
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/lift_controller_8.fsp

