#!/bin/sh

#SBATCH -N 1
#SBATCH --ntasks-per-node=1
#SBATCH -t 10:00
#SBATCH -p debug
#SBATCH --constraint=rhel8

cd /homes/cmsc714-1veb/parallel_astar

#. ~/.python-venvs/parallel-astar-env/bin/activate

python generate_graph.py