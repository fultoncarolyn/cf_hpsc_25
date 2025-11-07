//  ================================================================================
//  ||                                                                            ||
//  ||              solvers                                                       ||
//  ||              ------------------------------------------------------        ||
//  ||              S O L V E R S                                                 ||
//  ||                                                                            ||
//  ||              D E M O N S T R A T I O N   C O D E                           ||
//  ||              ------------------------------------------------------        ||
//  ||                                                                            ||
//  ||       Developed by: Scott R. Runnels, Ph.D.                                ||
//  ||                     University of Colorado Boulder                         ||
//  ||                                                                            ||
//  ||                For: CU Boulder CSCI 4576/5576 and associated labs          ||
//  ||                                                                            ||
//  ||           Copyright 2020 Scott Runnels                                     ||
//  ||                                                                            ||
//  ||                     Not for distribution or use outside of the             ||
//  ||                     this course.                                           ||
//  ||                                                                            ||
//  ================================================================================

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <string>
#include <vector>
#include <omp.h>
#include <chrono>
#include <ctime>
#include "stdio.h"
#include "math.h"
#include "string.h"

using std :: string;
using std :: vector;
using std :: stringstream;
using std :: cout;
using std :: endl;

#define rLOOP  for ( int r = 1 ; r <= nField    ; ++r )
#define cLOOP  for ( int c = 1 ; c <= bandwidth ; ++c )
#define iLOOP  for ( int i = 1 ; i <= nRealx    ; ++i )
#define jLOOP  for ( int j = 1 ; j <= nRealy    ; ++j )

#define rowLOOP for ( int row = 1; row <= nField ; ++row )
#define colLOOP for ( int col = 1; col <= bandwidth ; ++col )

#define matLOOP for ( int mat = 1; mat <= nMat ; ++mat )

typedef  vector<double>          VD;
typedef  vector<vector<double> > VDD;
typedef  vector<int>             VI;
typedef  vector<vector<int> >    VII;


void Exit()
{
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
  exit(0);
}


void FatalError(string msg)
{

  cout << " " << endl;
  cout << " " << endl;
  cout << "Fatal Error on this PE: " << msg << endl;
  cout << " " << endl;
  Exit();


}


// ==
// ||
// ||
// ||  timingInfo:  This class can be used to compute the execution time of the code or
// ||               any of its routines.
// ||
// ==

class timingInfo
{
 public:

  // Name of the intenty for which timing is desired

  string name;

  // Two methods of recording time:
  
  double       startSeconds ,  endSeconds ;  // (1)
  std::clock_t startCPUtime ,  endCPUtime ;  // (2)


  timingInfo()
    {
      name = "timingInfo name not set";
    }
    
  timingInfo(string _name)
    {
      name = _name;
    }
    
  void Start(int myPE)
  {

    // Record start time using two methods
    
    startSeconds = MPI_Wtime();              // (1)
    startCPUtime = std::clock();             // (2)
    
    if ( myPE == 0)
      {
	cout << "[timingInfo] [" << name << "] Start time (Wall): " << startSeconds << " Start time (CPU) : " << startCPUtime << endl;
      }

  }

  void Finish(int myPE)
  {

    // Record finish time using two methods
    
    endSeconds = MPI_Wtime();               // (1)
    endCPUtime = std::clock();              // (2)

    // Compute elapsed time
    
    double elapsedSeconds = endSeconds - startSeconds;  // (1)
    double elapsedCPUtime = endCPUtime - startCPUtime;  // (2)

    // Report results
    
    if ( myPE == 0)
      {

	// (1) results
	
	cout << "[timingInfo] [" << name << "] Finish time (Wall): " << endSeconds <<
	                                     " Finish time (CPU) : " << endCPUtime << endl;

	// (2) results
	
	cout << "[timingInfo] [" << name << "] Elapsed time (Wall, seconds): "  << elapsedSeconds <<
	                                     " Elapsed time (CPU, seconds) : "  << elapsedCPUtime/ (double) CLOCKS_PER_SEC << endl;
      }
    
    return ;
    
  }

};




float  * * Array2D_float(int nRows,int nCols)
{
  float *myArray;
  myArray = new float [ nRows * nCols ];
    
  // Create a pointer that points to the beginning of each new row

  float * * myArray_ptr;
  myArray_ptr = new float * [nRows];

  int count = 0;

  for ( int row = 0 ; row < nRows ; ++ row )
    {
      myArray_ptr[row] = &myArray[ count*nCols ];
      ++count;
    }

  // Return that pointer
  
  return myArray_ptr;

}

double  * * Array2D_double(int nRows,int nCols)
{
  double *myArray;
  myArray = new double [ nRows * nCols ];
    
  // Create a pointer that points to the beginning of each new row

  double * * myArray_ptr;
  myArray_ptr = new double * [nRows];

  int count = 0;

  for ( int row = 0 ; row < nRows ; ++ row )
    {
      myArray_ptr[row] = &myArray[ count*nCols ];
      ++count;
    }

  // Return that pointer
  
  return myArray_ptr;

}
