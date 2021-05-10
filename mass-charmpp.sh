#!/bin/bash

function build_submit() {
    bin=$1
    nprocs=$2
    
    node=1
    ntasks_per_node=$2
    if [ $nprocs -eq 32 ] ; then
        node=2
        ntasks_per_node=16
    fi
    run_dir="$(pwd)/charmpp"
    script_name=submit-${bin}-${node}-${nprocs}.sh


    echo "#!/bin/bash

#SBATCH -N $node
#SBATCH --ntasks-per-node=$ntasks_per_node
#SBATCH -t 05:00
#SBATCH -p debug
#SBATCH --constraint=rhel8

cd $run_dir

for m in openmpi/3.1.5/gcc/ charmpp ; do
    module load \$m
done

echo running ${bin} with ${node} nodes and ${nprocs} processes
mpirun -np $nprocs ./${bin}" > ${script_name} 
chmod +x ${script_name} 
sbatch ${script_name}
}

for bin in astar ; do
    for nprocs in 1 2 4 8 16 32 ; do
        build_submit $bin $nprocs
    done
done