#!/bin/bash
# run with params: dst_dir, output_dir, cltsa_dir, max_iters
echo "[[Running unrealizable examples]] with max iters $4"
# max it. 4
echo "[Lift controller]"
./run_missing_assumption_lift_controller_1_n.sh $1 $2 $3 4 $4 
./run_removed_env_safety_lift_controller_1_n.sh $1 $2 $3 4 $4 
# max it. 5
echo "[Collector]"
./run_missing_assumption_collector_1_n_in_v1.sh $1 $2 $3 5 $4 
./run_removed_env_safety_collector_1_n_in_v1.sh $1 $2 $3 5 $4 
# max it. 9
echo "[Exploration Robot]"
./run_missing_assumption_exploration_robot_1_n.sh $1 $2 $3 6 $4 
./run_removed_env_safety_exploration_robot_1_n.sh $1 $2 $3 6 $4 
# max it. 3
echo "[GenBuf]"
./run_missing_assumption_genbuf_1_n.sh $1 $2 $3 3 $4 
# max it. 2
./run_removed_env_safety_genbuf_1_n.sh $1 $2 $3 2 $4 
# max it. 3
echo "[AHB]"
./run_missing_assumption_ahb_1_n.sh $1 $2 $3 3 $4 
# max it. 3
./run_removed_env_safety_ahb_1_n.sh $1 $2 $3 3 $4 
