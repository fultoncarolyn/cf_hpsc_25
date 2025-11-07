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


//  ==
//  ||
//  ||      C L A S S:   L A P L A C I A N O N G R I D 
//  ||
//  == 

class LaplacianOnGrid
{

public:

  double x0, x1, y0, y1;
  VD x,y;
  int nRealx    , nRealy   , nField;
  int nCellx    , nCelly   ;
  int nStaggeredCellx, nStaggeredCelly;
  double dx, dy;
  VDD Acoef;
  VII Jcoef ;
  VD  phi ;  VD  b ;
  VD  phi_mms ;
  int bandwidth;
  int myPE;
  VD Qval;

  //  ==
  //  ||
  //  ||  Constructor:  Initialize values
  //  ||
  //  ==

  LaplacianOnGrid(double _x0   , double _x1,  double _y0, double _y1 , int ncell_x , int ncell_y, mpiInfo &myMPI )
  {

    x0 = _x0;    x1 = _x1;
    y0 = _y0;    y1 = _y1;

    nCellx     = ncell_x;
    nCelly     = ncell_y;

    nStaggeredCellx = nCellx - 1;
    nStaggeredCelly = nCelly - 1;
    
    nRealx     = ncell_x; 
    nRealy     = ncell_y; 
    nField  = nRealx*nRealy;  
    dx     = (x1-x0)/(ncell_x-1);
    dy     = (y1-y0)/(ncell_y-1);

    // Allocate memory -- Note that the node numbers and field variable numbers must
    // match.  So even though we only will be caring about real nodes, their node
    // numbers are naturally ordered, so must be of size nField.

    x.resize(nField+1); y.resize(nField+1); Qval.resize(nField+1);
		       
    for ( int i = 1 ; i <= nRealx ; ++i )
      for ( int j = 1 ; j <= nRealy ; ++j )
	{
	  int p = pid(i,j);
	  x[p] = x0 + (i-1)*dx;
	  y[p] = y0 + (j-1)*dy;
	}

    bandwidth = 5;
    
    Acoef.resize(nField+1 ); rLOOP Acoef[r].resize(bandwidth+1);
    Jcoef.resize(nField+1 ); rLOOP Jcoef[r].resize(bandwidth+1);
    b.resize(nField+1 );
    phi.resize(nField+1);
    phi_mms.resize(nField+1);

    for ( int p = 1 ; p <= nField ; ++p ) phi[p] = 0.;
    for ( int p = 1 ; p <= nField ; ++p ) phi_mms[p] = MMS01(p);
    
    myPE = myMPI.myPE;

  }

  //  ==
  //  ||
  //  ||  Form Linear System Ax = b
  //  ||
  //  ==

  void FormLS(mpiInfo &myMPI)
  {
    double half    = 0.5;
    double quarter = 0.25;
    
    rLOOP cLOOP Acoef[r][c] = 0.;  // Initialize linear system
    rLOOP cLOOP Jcoef[r][c] = 0 ;  // The default is to point to entry zero; note that phi[0] is set to 0, above.
    rLOOP       Jcoef[r][1] = r;   //
    rLOOP b[r] = 0.;
    
    double dx2 = dx*dx;           // Form matrix entries for the interior grid points
    double dy2 = dy*dy;           // Form matrix entries for the interior grid points

    // Set up the E/W/N/S stencil:

    for ( int i = 1 ; i <= nRealx ; ++i )
      for ( int j = 1 ; j <= nRealy ; ++j )
	{
	  int p = pid(i,j);
	                    Jcoef[ p ][ 1 ] =  p;
          if ( i < nRealx ) Jcoef[ p ][ 2 ] =  p+1;           // East
	  if ( i > 1      ) Jcoef[ p ][ 3 ] =  p-1;           // West
	  if ( j < nRealy ) Jcoef[ p ][ 4 ] =  p+nRealx;      // North
	  if ( j > 1      ) Jcoef[ p ][ 5 ] =  p-nRealx;      // South
	}

    // Loop over each cell, and let each cell contribute to its points' finite difference equation
    
    for ( int i = 1 ; i <= nStaggeredCellx ; ++i )
      for ( int j = 1 ; j <= nStaggeredCelly ; ++j )
	{
	  int p;


	  p = pid(i,j);
	  Acoef[p][1] += half * ( -1./dx2 - 1./dy2 );
	  Acoef[p][2] += half *    1./dx2;
	  Acoef[p][4] += half *    1./dy2;
	  b    [p]    += /* TO-DO in Lab */

	  p = pid(i+1,j);
	  Acoef[p][1] += half * ( -1./dx2 - 1./dy2 );
	  Acoef[p][3] += half *    1./dx2;
	  Acoef[p][4] += half *    1./dy2;
	  b    [p]    += /* TO-DO in Lab */

	  p = pid(i+1,j+1);
	  Acoef[p][1] += half * ( -1./dx2 - 1./dy2 );
	  Acoef[p][3] += half *    1./dx2;
	  Acoef[p][5] += half *    1./dy2;
	  b    [p]    += /* TO-DO in Lab */

	  p = pid(i,j+1);
	  Acoef[p][1] += half * ( -1./dx2 - 1./dy2 );
	  Acoef[p][2] += half *    1./dx2;
	  Acoef[p][5] += half *    1./dy2;
	  b    [p]    += /* TO-DO in Lab */
	  
      }

    // Apply BCs

    for ( int i = 1 ; i <= nRealx ; ++i )
      for ( int j = 1 ; j <= nRealy ; ++j )
	{
	  int p = pid(i,j);
	  if ( myMPI.iPE == 0            ) if ( i == 1      ) ApplyBC( p, MMS01(p)  );
	  if ( myMPI.iPE == myMPI.nPEx-1 ) if ( i == nRealx ) ApplyBC( p, MMS01(p)  );
	  if ( myMPI.jPE == 0            ) if ( j == 1      ) ApplyBC( p, MMS01(p)  );
	  if ( myMPI.jPE == myMPI.nPEy-1 ) if ( j == nRealy ) ApplyBC( p, MMS01(p)  );
	}
    
  }

  void ApplyBC(int BCrow,double BCvalue)
  {
    cLOOP Acoef[BCrow][c] = 0.; 
    cLOOP Jcoef[BCrow][c] = 0 ;

    Acoef[ BCrow ] [ 1 ] = 1.      ; 
    Jcoef[ BCrow ] [ 1 ] = BCrow   ;
    b    [ BCrow ]       = BCvalue ;

    rLOOP
      if (  r != BCrow )
	{
	  cLOOP
	    {
	      if ( Jcoef[r][c] == BCrow )
		{
		  b[r] -= Acoef[r][c]*BCvalue;
		  Jcoef[r][c] = 0; Acoef[r][c] = 0.;
		}
	    }
	}
  }

  //  ==
  //  ||
  //  ||  Method of Manufactured Solutions
  //  ||
  //  ==

  double MMS01(int p)
  {
    /* TO-DO in Lab: After successful linear testing, upgrade to cubic */
    return x[p]* + y[p];
  }

  double MMS01_f(int p)
  {
    /* TO-DO in Lab: After successful linear testing, upgrade to cubic */
    return 0.;
  }

  
  //  ==
  //  ||
  //  ||  Utility routines
  //  ||
  //  ==

  void printLinearSystem(int myPE)
  {

    rLOOP
      {
	printf("myPE: %d row = %3d: ",myPE,r);
	cLOOP printf(" | %2d--> %9.5e ",Jcoef[r][c],Acoef[r][c] );

      }

  }

  int pid(int i,int j) { return i + (j-1)*nRealx; }  
                                                       
  #include "plotter.h"
  #include "linearSolver.h"
};


