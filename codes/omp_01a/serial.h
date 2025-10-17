  StartTimer(t0);

  double * *   Acoef1 = Array2D_double(nField,bandwidth);
  int    * *   Jcoef1 = Array2D_int   (nField,bandwidth);

  for ( int row = 0 ; row < nField ; ++row )
    for ( int col = 0 ; col < bandwidth ; ++col )
      {
	Acoef1[row][col] = 0.;
	Jcoef1[row][col] = 0 ;
      }
  
  for ( int row = 0 ; row < nField ; ++row )
    {
      Acoef1 [row][0] = -4.;
      Jcoef1 [row][0] = row;
      phi[row]      =  1.;

      if ( row > 0 && row < nField-1 ) { Acoef1[row][1] = 1 ; Acoef1[row][2] = 1;  Jcoef1[row][1] = row-1 ; Jcoef1[row][2] = row+1;}
    }

    #include "repeat.h"
    for ( int row = 0 ; row < nField ; ++row )
      {
	prod[row] = 0.;
	for ( int col = 0 ; col < bandwidth ; ++col )
	  {
	    prod[row] += Acoef1[row][col] * phi[ Jcoef1[row][col]];
	  }
      }

  EndTimer("serial          ", t0,t1);

  printArray(nField,prod);
  
delete [] Acoef1;
delete [] Jcoef1;
