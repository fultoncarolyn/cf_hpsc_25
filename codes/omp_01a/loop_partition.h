  StartTimer(t0);
  
  double * *   Acoef3 = Array2D_double(nField,bandwidth);
  int    * *   Jcoef3 = Array2D_int   (nField,bandwidth);
  
  #pragma omp parallel
  {
    int myTH = omp_get_thread_num();
    
    // This thread's bounds

    int numPerTH = nField/numTH;
    int Lower    = myTH*numPerTH;
    int Upper    = Lower + numPerTH - 1;

    if ( myTH == numTH-1) Upper = nField - 1;

    // Initialize matrix

    for ( int row = Lower ; row <= Upper ; ++row )
      for ( int col = 0 ; col < bandwidth ; ++col )
	{
	  Acoef3[row][col] = 0.;
	  Jcoef3[row][col] = 0;
	}
  
    // Populate matrix

    for ( int row = Lower ; row <= Upper ; ++row ) 
    { 
      Acoef3[row][0] = -4.;
      Jcoef3[row][0] = row;
      phi  [row]    =  1.; 
      if ( row > 0 && row < nField-1 ) { Acoef3[row][1] = 1 ; Acoef3[row][2] = 1;  Jcoef3[row][1] = row-1 ; Jcoef3[row][2] = row+1;}
    }
    
    // Matrix-vector multiply
    
    #include "repeat.h"
    for ( int row = Lower ; row <= Upper ; ++row ) 
      { 
     	prod[row] = 0.; 
     	for ( int col = 0 ; col < bandwidth ; ++col ) 
    	  { 
     	    prod[row] += Acoef3[row][col] * phi[ Jcoef3[row][col] ]; 
     	  } 
      } 
  }

  EndTimer("loop_partition  ", t0,t1);

  printArray(nField,prod);

delete [] Acoef3;
delete [] Jcoef3;
