
# -----------------------------------------------------
# 
#    This is a
#
#    M A K E F I L E
# 
# -----------------------------------------------------
# 
# This is a "makefile".  Makefiles are used with the
# program "make", which is executed at the command line
# by entering the command "make".  The purpose of the
# makefile and using make is to relieve the developer
# of the responsibility of remembering which files,
# if any, need to be compiled in order to form the
# executable ("binary").  The Makefile specifies on
# which of the source code files the binary depends
# and also specifies the command to be used to 
# compile those source code files.
# 
# Continue to read down this file to understand each
# part.

# -----------------------------------------------------
# First, load modules:
# -----------------------------------------------------
#
# First, before you can expect the compiler, specified
# below, to work on Alpine, you will need to load the 
# "intel" module.  Once on an Alpine compile node, enter 
# this command:
#
# module load intel

# -----------------------------------------------------
# Set the compiler
# -----------------------------------------------------

COMPILER := mpicc

# -----------------------------------------------------
# Specify dependency and compile command
# -----------------------------------------------------

# The first line shows "make" that the executable
# "lookup" depends on the source code file
# "lookup.cpp".  The second line, which *must* be
# indented using a TAB, specifies the compile command.

lookup: lookup.cpp 
	$(COMPILER) lookup.cpp  -lstdc++ -o lookup

