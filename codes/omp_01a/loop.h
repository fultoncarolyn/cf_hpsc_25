  StartTimer(t0);

  double * *   Acoef2 = Array2D_double(nField,bandwidth);
  int    * *   Jcoef2 = Array2D_int   (nField,bandwidth);

  #pragma omp parallel for
  for ( int row = 0 ; row < nField ; ++row )
    for ( int col = 0 ; col < bandwidth ; ++col )
      {
	Acoef2[row][col] = 0.;
	Jcoef2[row][col] = 0 ;
      }
  
  #pragma omp parallel for
  for ( int row = 0 ; row < nField ; ++row )
    {
      Acoef2 [row][0] = -4.;
      Jcoef2 [row][0] = row;
      phi[row]      =  1.;

      if ( row > 0 && row < nField-1 ) { Acoef2[row][1] = 1 ; Acoef2[row][2] = 1;  Jcoef2[row][1] = row-1 ; Jcoef2[row][2] = row+1;}
    }

   #include "repeat.h"
   #pragma omp parallel for
    for ( int row = 0 ; row < nField ; ++row )
      {
	prod[row] = 0.;
	for ( int col = 0 ; col < bandwidth ; ++col )
	  {
	    prod[row] += Acoef2[row][col] * phi[ Jcoef2[row][col]];
	  }
      }

  EndTimer("loop            ", t0,t1);

  printArray(nField,prod);
  
delete [] Acoef2;
delete [] Jcoef2;
