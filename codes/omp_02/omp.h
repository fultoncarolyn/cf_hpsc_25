#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <string>
#include <vector>
#include "math.h"
#include <omp.h>  
#include <mpi.h>  
#include <stdio.h>
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

typedef  vector<double>          VD;
typedef  vector<vector<double> > VDD;
typedef  vector<int>             VI;
typedef  vector<vector<int> >    VII;


double  * Array1D_double(int nRows)
{
  double *myArray;
  myArray = new double [ nRows ];
  return myArray;

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

int  * * Array2D_int(int nRows,int nCols)
{
  int *myArray;
  myArray = new int [ nRows * nCols ];
    
  // Create a pointer that points to the beginning of each new row

  int * * myArray_ptr;
  myArray_ptr = new int * [nRows];

  int count = 0;

  for ( int row = 0 ; row < nRows ; ++ row )
    {
      myArray_ptr[row] = &myArray[ count*nCols ];
      ++count;
    }

  // Return that pointer
  
  return myArray_ptr;

}









// ====================================================================================
// ||                                                                                ||
// ||                      T i m i n g   U t i l i i t e s                           ||
// ||                                                                                ||
// ====================================================================================

void StartTimer(struct timespec &t0  )
{
  clock_gettime(CLOCK_REALTIME, &t0);
}

void EndTimer( string KernelName , struct timespec &t0 , struct timespec &t1 )
{
  // The timespec struct provides time information in this format:
  //
  // tv_sec:tv_nsec  
  //
  // which should be thought of as an anology to the more commonly understood
  // time format
  //
  //  HH:MM:SS
  //
  // So in order to compute elapsed time, one must subtract each place (e.g., HH, MM, SS),
  // and add the differences while including units conversion

  clock_gettime(CLOCK_REALTIME, &t1);

  int64_t sec_place     = t1.tv_sec  - t0.tv_sec ;
  int64_t ns_place      = t1.tv_nsec - t0.tv_nsec;
  int64_t sec_2_ns      = 1000000000;
  int64_t exeTime_in_ns = sec_place*sec_2_ns + ns_place;
  int64_t exeTime_in_ms = exeTime_in_ns / 1000;
  std::cout.imbue(std::locale("en_US.UTF-8")); 
  cout << "[ " << KernelName << " ] EXECUTION TIME  = " << exeTime_in_ms << " (ms)" << endl;

}


void plot(string descriptor, double *field, int nPtsx, int nPtsy, double dh)
  {
    // Open plot file
    
    string filename = descriptor + ".plt";    std::fstream f;  f.open(filename.c_str(),std::ios::out);

    // Write to plot file

    for ( int i = 0 ; i < nPtsx ; ++i )
      {
	for ( int j = 0 ; j < nPtsy; ++j )
	  {
	    int p = i + j*nPtsx;
	    f << i*dh << " " << j*dh << " " << field[ p ] << endl;
	  }
	f << endl;
	//	f << " 0  0  " << field[ 0 ] << endl;
      }

    f.close();
  }




