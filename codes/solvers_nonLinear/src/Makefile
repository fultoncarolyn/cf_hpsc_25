
# -----------------------------------------------------
# On local Ubuntu
# -----------------------------------------------------
#
# Be sure to use MPI compiler wrappers.
# Whenever compiling an MPI program, you should use the MPI wrappers:
#
#    C - mpicc
#    C++ - mpiCC, mpicxx, mpic++
#    FORTRAN - mpifort, mpif77, mpif90
#
# These wrappers do all of the dirty work for you of making sure that
# all of the appropriate compiler flags, libraries, include directories,
# library directories, etc. are included when you compile your program.


# -----------------------------------------------------
# On Alpine:
# -----------------------------------------------------
#
#module purge
#module load intel
#module load intel/2022.1.2
#module load advisor/2022.0.0
#module load impi

# -----------------------------------------------------
# Set compiler based on platform
# -----------------------------------------------------

# Default compiler is Intel MPI compiler

COMPILER := mpicc

# Default ADVINCLUDE path is for Alpine
ADVINCLUDE := /curc/sw/install/intel/2022.1.2/advisor/2022.0.0/include

# Change compiler and ADVINCLUDE if on my local system

ifeq ($(strip $(LOGNAME)),scott)
COMPILER := mpic++ 
ADVINCLUDE := /home/scott/intel/oneapi/advisor/2024.3/include
endif


# -----------------------------------------------------
# Make solvers
#
# Notes:
#
# (1) the -I and -ldl are for Intel Advisor annotations
#
# -----------------------------------------------------


solvers: solvers.cpp solvers.h linearSolver.h plotter.h mpiInfo.h LaplacianOnGrid.h
	$(COMPILER) solvers.cpp -g -o solvers -std=c++11 -fopenmp -I $(ADVINCLUDE) -ldl


