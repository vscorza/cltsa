#!/bin/bash
echo "[[Running unrealizable examples]]"
./run_missing_assumption_collector_1_n_in_v1.sh
./run_missing_assumption_exploration_robot_1_n.sh
./run_missing_assumption_genbuf_1_n.sh
./run_removed_env_safety_genbuf_1_n.sh
