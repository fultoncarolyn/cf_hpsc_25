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


// ====================================================================================
// ||                                                                                ||
// ||                      T i m i n g   U t i l i i t e s                           ||
// ||                                                                                ||
// ====================================================================================

void StartTimer(struct timespec &t0  )
{
  //  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t0);
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

  //  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1);
  clock_gettime(CLOCK_REALTIME, &t1);

  int64_t sec_place     = t1.tv_sec  - t0.tv_sec ;
  int64_t ns_place      = t1.tv_nsec - t0.tv_nsec;
  int64_t sec_2_ns      = 1000000000;
  int64_t exeTime_in_ns = sec_place*sec_2_ns + ns_place;
  int64_t exeTime_in_ms = exeTime_in_ns / 1000;

  cout << KernelName << " EXECUTION TIME  = " << exeTime_in_ms << " (ms)" << endl;

}

