#!/bin/bash

function build_submit() {
    bin=$1
    nprocs=$2
    K=$3

    node=1
    ntasks_per_node=$2
    if [ $nprocs -eq 32 ] ; then
        node=2
        ntasks_per_node=16
    fi
    run_dir="$(pwd)/src"
    nodes_file="$(pwd)/test_graphs-${K}/nodes.out"
    edges_file="$(pwd)/test_graphs-${K}/edges.out"
    src_dst_file="$(pwd)/test_graphs-4/src_dst.out"
    script_name=submit-${bin}-${node}-${nprocs}-${K}.sh


    echo "#!/bin/bash

#SBATCH -N $node
#SBATCH --ntasks-per-node=$ntasks_per_node
#SBATCH -t 05:00
#SBATCH -p debug
#SBATCH --constraint=rhel8

cd $run_dir

for m in openmpi/3.1.5/gcc/ ; do
    module load \$m
done

echo running ${bin} with ${node} nodes, ${nprocs} processes, and K=${K}
mpirun -np $nprocs ./${bin} ${nodes_file} ${edges_file} ${src_dst_file}" > ${script_name} 
chmod +x ${script_name} 
sbatch ${script_name}
}

for bin in astar_mpi ; do
    for nprocs in 1 2 4 8 16 32 ; do
        for K in 4 ; do
            build_submit $bin $nprocs $K
        done
    done
done