  StartTimer(t0);
  
  #pragma omp parallel
  {
    int myTH = omp_get_thread_num();
    
    // This thread's bounds

        int numPerTH = nField/numTH;
        int Lower    = myTH*numPerTH;
        int Upper    = Lower + numPerTH - 1;

        if ( myTH == numTH-1) Upper = nField - 1;

    // Get local memory

        int nField_TH = Upper - Lower + 1;

        double * * Acoef_TH = Array2D_double(nField_TH,bandwidth);
        int    * * Jcoef_TH = Array2D_int   (nField_TH,bandwidth);
    
    // Initialize matrix
    
        for ( int row = 0 ; row < nField_TH; ++row )
          for ( int col = 0 ; col < bandwidth ; ++col )
    	{
	  	  Acoef_TH[row][col] = 0.;
	  	  Jcoef_TH[row][col] = 0;
	}
    
    // Populate matrix
    
        for ( int row = Lower ; row <= Upper ; ++row )
            {
	      	int row_TH = row - Lower;
		Acoef_TH[row_TH ][0] = -4.;
		Jcoef_TH[row_TH ][0] = row;
		phi     [row    ]    =  1.;
		if ( row > 0 && row < nField-1 ) { Acoef_TH[row_TH][1] = 1 ; Acoef_TH[row_TH][2] = 1;  Jcoef_TH[row_TH][1] = row-1 ; Jcoef_TH[row_TH][2] = row+1;}
	      }
    
    // Matrix-vector multiply

	#include "repeat.h"
        for ( int row = Lower ; row <= Upper ; ++row )
          {
	      	int row_TH = row - Lower;
	      	prod[row] =  0.;
	      	for ( int col = 0 ; col < bandwidth ; ++col )
	      	  {
              	    prod[row] += Acoef_TH[row_TH][col] * phi[ Jcoef_TH[row_TH][col] ];
	      	  }
	      }
delete [] Acoef_TH;
delete [] Jcoef_TH;
  }

  EndTimer("memory_partition", t0,t1);

  printArray(nField,prod);

