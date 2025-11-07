#!/bin/bash

# -
# |
# | This is a batch script for running a MPI parallel job on Summit
# |
# | (o) To submit this job, enter:  sbatch slurm.bat 
# |
# | (o) To check the status of this job, enter: squeue -u <username>
# |
# -

# -
# |
# | Part 1: Directives
# |
# -

#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --time=00:03:00
#SBATCH --partition=atesting
#SBATCH --qos=testing
#SBATCH --output=slurm.out

# -
# |
# | Part 2: Loading software
# |
# -

module purge
module load intel
module load intel/2022.1.2
module load advisor/2022.0.0
module load impi


# -
# |
# | Part 3: User scripting
# |
# -

echo "=="
echo "||"
echo "|| Begin Execution of run in slurm batch script."
echo "||"
echo "=="

# IMPORTANT See the "run" file for "./solvers" full command-line arguments.  You will want to include
#           all of the "./solvers" command-line arguments when you run the cases below.

# -------------------------------
# To run a "Survey"
# -------------------------------

# srun -n 1 advixe-cl --collect=survey --project-dir=./ --search-dir src:r=./ -- ./solvers -nPEx 1 -nPEy 1 -nCellx 10 -nCelly 10 -solver cg -nl nr

# -------------------------------
# To run a "Suitability Study"
# -------------------------------

# srun -n 1 advixe-cl --collect=suitability --project-dir=./ --search-dir src:r=./ -- ./solvers -nPEx 1 -nPEy 1 -nCellx 10 -nCelly 10 -solver cg -nl nr
 
echo "=="
echo "||"
echo "|| Execution of run in slurm batch script complete."
echo "||"
echo "=="








