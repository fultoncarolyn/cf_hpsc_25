#include "omp.h"

// ==
// ||
// ||  OMP Laplacian Demo Code
// ||
// ==


int main(  int argc, char *argv[] ) 
{

  // Start the timer for the entire execution

  struct timespec t0, t1;
  StartTimer(t0);


  // Default values for user input
    
  int nCell = 190;  // nCell is for the x- and y-directions
  int numTH = 1;   // Number of threads

  // Gather user input from the command line

  for (int count =  0 ; count < argc; ++count)
    {
      if ( !strcmp(argv[count],"-nCell"    ) ) nCell     = atoi(argv[count+1]);  
      if ( !strcmp(argv[count],"-nTH"      ) ) numTH     = atoi(argv[count+1]);
    }

  // Set the number of threads for this execution

  omp_set_num_threads(numTH);

  // Set up grid and linear system information

  double dh          = 1./nCell;        // Size of each cell in each direction (dx and dy)
  int nPtsx          = nCell+1;         // Number of points in the x-direction
  int nPtsy          = nCell+1;         //   "    "    "    "   "  y-    "
  int nField         = nPtsx * nPtsy;   // Total number of points (and number of rows in the linear system)
  int bandwidth      = 5;               // Number of columns in the sparse-matrix format
  int max_iter       = nField*10;       // Number of allowed Jacobi iterations
  int numTHconverged = 0;               // Number of threads converged
    
  double   * phi;                       // The unknown for which we are solving A*phi = b
  
  phi     = Array1D_double(nField);     // Acquire memory for this shared variable.

  // This shared array will store either a 0 or 1 for each thread, 0 meaning not congverged
  // 1 meaning converged.
  
  int *THconverged = new int [ numTH ];   // records which thread(s) have converged
  

// =================================================================================== //
// =======================         BEGIN PARALLEL REGION          ==================== //
// =================================================================================== //
  
#pragma omp parallel //shared(numTHconverged)
  {
    int myTH = omp_get_thread_num();  

    // ------------------------------------------------------------------------------------
    // (1) Compute this thread's bounds in the global system:
    //
    //     numPerTH = number of rows per thread (the last thread may be slightly different)
    //     Lower    = the first row in the global matrix to be handled by this thread
    //     Upper    = the last   "  "   "    "      "    "  "    "     "   "      "
    // ------------------------------------------------------------------------------------

    int numPerTH = nField/numTH;                       // ~LabReplace~   ~nField/numTH~          ~   TO-DO  ~
    // improvement - remove following two lines
    //int Lower    = myTH*numPerTH;                      // ~LabReplace~   ~myTH*numPerTH~         ~   TO-DO  ~
    //int Upper    = Lower + numPerTH - 1;               // ~LabReplace~   ~Lower + numPerTH - 1~  ~   TO-DO  ~

    // add following
    int remainder = nField - numPerTH*numTH

    //  (1.1) Adjust Upper for the last (highest numbered) thread to ensure all the rows of the global system are handled
    
    // improve - remove then add ...
    // if ( myTH == numTH-1) Upper = nField - 1;         // ~LabStrip~
     
     // ... initialize
     int Lower = 0;
     int Upper = 0;

     // if in the first ``remainder" threads, assign numPerTH + 1 rows
if ( myTH + 1 <= remainder )
{
    Lower = myTH*(numPerTH+1);
    Upper = (myTH+1)*(numPerTH+1) - 1;
}
// else assign numPerTH rows
if ( myTH + 1 > remainder )
{
    Lower = (nField - 1) - (numTH - myTH)*(numPerTH) + 1;
    Upper = (nField - 1) - (numTH - (myTH+1))*(numPerTH);
}

cout << myTH << " : Lower = "<< Lower << " Upper = " << Upper << endl;
    // ------------------------------------------------------------------------------------
    // (2) Aquire memory for this thread
    //
    //     nField_TH = the number of field variables (phi) to be handled by this thread.
    //                 It is computed using the values computed in the lines above.
    // ------------------------------------------------------------------------------------

    int nField_TH = (Upper - Lower) + 1

    double * * Acoef;
    int    * * Jcoef;
    double   * b;
    double   * phiNew;

    //     Acoef, Jcoef, b, and phiNew are not the global matrices and arrays; here they contain only the
    //     rows being handled by this thread.  They are dimensioned accordingly.
    
    Acoef    = Array2D_double( nField_TH, bandwidth); 
    Jcoef    = Array2D_int   ( nField_TH, bandwidth); 
    b        = Array1D_double( nField_TH); 
    phiNew   = Array1D_double(  nField_TH); 
    
    // ------------------------------------------------------------------------------------
    // (3) Initialize the linear system and form the initial guess for phi
    // ------------------------------------------------------------------------------------

    // Here we loop all the rows (points) on this thread

    for ( int row = 0 ; row < nField_TH ; ++row )  // loop over each threads local matrix using upper limit
      {
	for ( int col = 0 ; col < bandwidth ; ++col )
	  {
	    Acoef[row][col] = 0.;
	    Jcoef[row][col] = 0 ;
	  }
	b[row] = 0.;
      }
    
    for ( int row = Lower ; row <= Upper; ++row )    // Fill in the upper limit of this thread's rows
      {
	phi[row] = 0.;
      }
    
    // ------------------------------------------------------------------------------------
    // (4) Form the linear system.  Here "pt" represents "point" number in the mesh.  It is
    //     equal to the row number in the linear system, too.
    // ------------------------------------------------------------------------------------
    
    for ( int pt = Lower ; pt <= Upper ; ++pt )    
      {

	// Using the same logic as for converting myPE to iPE and jPE,
	// compute the i,j logical coordinates of "pt" in the mesh:
	
	int j = int( pt/nPtsx ); 
	int i = pt - j*nPtsx;

	// Compute the row number local to this thread, relative to its Acoef/Jcoef arrays.
	// In other words, convert the global point number "pt" to the thread's local point
	// number "pt_TH".
	
	int pt_TH = pt - Lower; 

	// Populate the linear system for all interior points using that local row number

	if ( i > 0 && i < nPtsx-1 )
	  if ( j > 0 && j < nPtsy-1 )
	    {
	      Acoef[pt_TH][0] = -4./dh/dh;  Jcoef[pt_TH][0] = pt;        
	      Acoef[pt_TH][1] =  1./dh/dh;  Jcoef[pt_TH][1] = pt - 1;    
	      Acoef[pt_TH][2] =  1./dh/dh;  Jcoef[pt_TH][2] = pt + 1;    
	      Acoef[pt_TH][3] =  1./dh/dh;  Jcoef[pt_TH][3] = pt + nPtsx;
	      Acoef[pt_TH][4] =  1./dh/dh;  Jcoef[pt_TH][4] = pt - nPtsx;
	    }

	// Apply boundary conditions
	
	if ( i == 0       ) { Acoef[pt_TH][0] = 1. ; Jcoef[pt_TH][0] = pt; b[pt_TH] =  1.; } 
	if ( i == nPtsx-1 ) { Acoef[pt_TH][0] = 1. ; Jcoef[pt_TH][0] = pt; b[pt_TH] = -1.; } 
	if ( j == 0       ) { Acoef[pt_TH][0] = 1. ; Jcoef[pt_TH][0] = pt; b[pt_TH] = -1.; } 
	if ( j == nPtsy-1 ) { Acoef[pt_TH][0] = 1. ; Jcoef[pt_TH][0] = pt; b[pt_TH] =  1.; } 
      }

    // ------------------------------------------------------------------------------------
    // (5) Perform Jacobi iterations
    // ------------------------------------------------------------------------------------

    int iter       = 0;
    int thisThreadConverged;

    #pragma omp barrier

    // numTHconverged counts the number of threads converged.  We do not care which thread initalizes it.
    
    #pragma omp single   
    {
      numTHconverged = 0;
    }

    // Iterate until all of the threads have converged or we have exceeded the number of allowed iterations
  
    while ( numTHconverged < numTH && ++iter <= max_iter )  
      {
	thisThreadConverged = 1;

	for ( int row = Lower ; row <= Upper ; ++row )     
	  {
	    int row_TH = row - Lower; 
	    
	    phiNew[row_TH] = b[row_TH]; 
	    
	    for ( int col = 1 ; col < bandwidth ; ++col ) // fixed brackets here...
      {
          phiNew[row_TH] -= Acoef[row_TH][col] * phi[ Jcoef[row_TH][col] 
          ]; 
      } 
	    phiNew[row_TH] /= Acoef[row_TH][0]; 
	    
	    if ( fabs(phiNew[row_TH] -  phi[row] ) > 1.e-10 ) 
      thisThreadConverged = 0;

	  }

	// (5.1) Record in shared array if this thread converged or not
	
	THconverged[myTH] = thisThreadConverged; 

	// (5.2) Count the number of threads that have converged.  We do not care which thread does the counting.
        #pragma omp barrier
        #pragma omp parallel /* Add a clause for efficiecy */   
        // #pragma omp single 
	{
	  numTHconverged = 0;
	  for ( int i = 0 ; i < numTH ; ++i ) numTHconverged += THconverged[i]; 
	  if ( numTHconverged == numTH ) printf("Jacobi converged in %d iterations.",iter);
	}

	// (5.3) Update the shared/global array phi with this thread's values
	
	for ( int row = Lower ; row <= Upper ; ++row )   
	  {
	    int row_TH = row - Lower;  
	    phi[row] = phiNew[row_TH]; 
	  }
      #pragma omp barrier
      }

  }

// =================================================================================== //
// =======================           END PARALLEL REGION          ==================== //
// =================================================================================== //
  
  if ( numTHconverged != numTH ) printf("WARNING: Jacobi did not converge.\n");
  
  EndTimer("main", t0,t1);
  
  plot("phi",phi,nPtsx,nPtsy,dh);
  return 0;
}

