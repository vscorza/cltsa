#!/bin/bash
# run with params: dst_dir, output_dir, cltsa_dir, max_iters
echo "[[Running realizable examples]]"
# max it. 4
echo "[Lift controller]"
./run_realizable_lift_controller_1_n.sh $1 $2 $3 4 $4 
# max it. 5
echo "[Collector]"
./run_realizable_collector_1_n.sh $1 $2 $3 5 $4
# max it. 9
echo "[Exploration Robot]"
./run_realizable_exploration_robot_1_n.sh $1 $2 $3 6 $4
# max it. 3
echo "[GenBuf]"
./run_realizable_genbuf_1_n.sh $1 $2 $3 3 $4
# max it. 3
echo "[AHB]"
./run_realizable_ahb_1_n.sh $1 $2 $3 3 $4

