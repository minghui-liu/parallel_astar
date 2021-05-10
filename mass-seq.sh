#!/bin/bash

function build_submit() {
    bin=$1
    K=$2
    
    run_dir="$(pwd)/src"
    nodes_file="$(pwd)/test_graphs-${K}/nodes.out"
    edges_file="$(pwd)/test_graphs-${K}/edges.out"
    src_dst_file="$(pwd)/test_graphs-4/src_dst.out"
    script_name=submit-${bin}-${K}.sh


    echo "#!/bin/bash

#SBATCH -N 1
#SBATCH --ntasks-per-node=1
#SBATCH -t 05:00
#SBATCH -p debug
#SBATCH --constraint=rhel8

cd $run_dir

module load gcc

echo running ${bin} with K = ${K}
./${bin} ${nodes_file} ${edges_file} ${src_dst_file}" > ${script_name} 
chmod +x ${script_name} 
sbatch ${script_name}
}

for bin in astar_seq ; do
    for K in 4 ; do
        build_submit $bin $K
    done
done