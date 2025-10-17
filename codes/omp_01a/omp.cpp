#include "omp.h"

void printArray(int N,double *prod)
{
  //  for ( int i = 0 ; i < N ; ++i ) cout << "prod[" << i << "] = " << prod[i] << endl;
}


// ==
// ||
// ||  M A I N
// ||
// ==


int main(  int argc, char *argv[] )
{
  struct timespec t0, t1;

  int numTH = 4;
  omp_set_num_threads(numTH);

  // Size of matrix
  
  int nField = 5000;
  
  double   * phi  = Array1D_double(nField);
  double   * prod = Array1D_double(nField);
  int bandwidth = 5.;

  for ( int row = 0 ; row < nField ; ++row )
    {
      phi [row]      =  1.123;
      prod[row]      =  0.;
    }

  // =========================================== LOOP SERIAL

  #include "serial.h"
  
  // =========================================== LOOP PARALLEL
  
  #include "loop.h"

  // =========================================== HIGH-LEVEL PARALLEL: LOOP PARTITIONING
  
  #include "loop_partition.h"
  
  // =========================================== HIGH-LEVEL PARALLEL: ARRAY PARTITIONING

  #include "memory_partition.h"
 
  return 0;
  
}


