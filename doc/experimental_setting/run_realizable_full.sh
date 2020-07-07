#!/bin/bash
# run with params: dst_dir, output_dir, cltsa_dir, max_iters
echo "[[Running realizable examples]]"
# max it. 4
./run_realizable_lift_controller_1_n.sh $1 $2 $3 4 $4 
# max it. 5
./run_realizable_collector_1_n.sh $1 $2 $3 5 $4
# max it. 9
./run_realizable_exploration_robot_1_n.sh $1 $2 $3 9 $4
# max it. 3
./run_realizable_genbuf_1_n.sh $1 $2 $3 3 $4


