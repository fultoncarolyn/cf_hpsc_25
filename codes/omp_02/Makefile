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
#
# -----------------------------------------------------
# On Summit:
# -----------------------------------------------------
#
# module load intel
# module load impi

# -----------------------------------------------------
# Set compiler based on platform
# -----------------------------------------------------

# Default compiler is Intel MPI compiler

COMPILER := mpicc
UBUNTU   := ubuntu

# Change if on ubuntu

#ifeq ($(strip $(SESSION)),$(UBUNTU))
COMPILER := mpicxx
#endif

# -----------------------------------------------------
# Make main
# -----------------------------------------------------

#omp_10: omp_10.cpp omp.h
#	$(COMPILER) -fopenmp omp_10.cpp -o omp_10 -std=c++11

omp: omp.cpp omp.h Makefile
	$(COMPILER) -O3 -fopenmp omp.cpp -o omp -std=c++11



# optimized	$(COMPILER) -O3 -fopenmp omp.cpp -o omp -std=c++11



