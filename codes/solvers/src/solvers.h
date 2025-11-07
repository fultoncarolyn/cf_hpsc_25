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




