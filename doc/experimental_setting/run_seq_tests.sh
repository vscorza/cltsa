#!/bin/bash
OUTPUT_DIR=$1
CLTSA_DIR=$2

echo "::[Running seq tests]::"
cd ${CLTSA_DIR}
echo "[AHB]"
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/ahb_arbiter_2_sched.fsp Ahb.arbiter.2
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/ahb_arbiter_3_sched.fsp Ahb.arbiter.3
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/ahb_arbiter_4_sched.fsp Ahb.arbiter.4
echo "[Collector]"
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/collector_1_in_v1.fsp Collector.1
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/collector_2_in_v1.fsp Collector.2
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/collector_3_in_v1.fsp Collector.3
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/collector_4_in_v1.fsp Collector.4
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/collector_5_in_v1.fsp Collector.5
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/collector_6_in_v1.fsp Collector.6
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/exploration-robot-v1_10.fsp Exploration.Robot.10
echo "[Exploration Robot]"
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/exploration-robot-v1_20.fsp Exploration.Robot.20
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/exploration-robot-v1_30.fsp Exploration.Robot.30
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/exploration-robot-v1_40.fsp Exploration.Robot.40
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/exploration-robot-v1_50.fsp Exploration.Robot.50
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/exploration-robot-v1_100.fsp Exploration.Robot.100
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/exploration-robot-v1_150.fsp Exploration.Robot.150
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/exploration-robot-v1_200.fsp Exploration.Robot.200
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/exploration-robot-v1_250.fsp Exploration.Robot.250
echo "[Genbuf]"
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/genbuf_1_sndrs_no_automaton.fsp GenBuf.1
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/genbuf_2_sndrs_no_automaton.fsp GenBuf.2
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/genbuf_3_sndrs_no_automaton.fsp GenBuf.3
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/genbuf_4_sndrs_no_automaton.fsp GenBuf.4
echo "[Lift Controller]"
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/lift_controller_2.fsp Lift.Controller.2
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/lift_controller_4.fsp Lift.Controller.4
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/lift_controller_6.fsp Lift.Controller.6
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/lift_controller_8.fsp Lift.Controller.8
nice -20 ./cltsa -r -o $OUTPUT_DIR tests/seq_tests/lift_controller_10.fsp Lift.Controller.10
