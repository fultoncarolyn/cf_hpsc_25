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
  VDD Jacobian ;  
  VD  phi ;  VD  b ;  VD minusf ; VD dPhi; VD phiNew; 
  int bandwidth;
  int myPE;
  VD Qval;
  double k0 , k1, k2;
  double pi;

  //  ==
  //  ||
  //  ||  Constructor:  Initialize values
  //  ||
  //  ==

  LaplacianOnGrid(double _x0   , double _x1,  double _y0, double _y1 , int ncell_x , int ncell_y, mpiInfo &myMPI )
  {

    k0 =  2.0 ;
    k1 =  0.0 ;
    k2 =  0.0 ;

    x0 = _x0;    x1 = _x1;
    y0 = _y0;    y1 = _y1;

    nCellx     = ncell_x;
    nCelly     = ncell_y;

    nStaggeredCellx = nCellx - 1;
    nStaggeredCelly = nCelly - 1;
    
    nRealx     = ncell_x; 
    nRealy     = ncell_y; 
    nField     = nRealx*nRealy;  
    dx         = (x1-x0)/(ncell_x-1);
    dy         = (y1-y0)/(ncell_y-1);

    // Form the mesh

    x.resize(nField+1); y.resize(nField+1); Qval.resize(nField+1);
		       
    for ( int i = 1 ; i <= nRealx ; ++i )
      for ( int j = 1 ; j <= nRealy ; ++j )
	{
	  int p = pid(i,j);
	  x[p] = x0 + (i-1)*dx;
	  y[p] = y0 + (j-1)*dy;
	}

    // Allocate memory for matrices and field variables
    
    bandwidth = 5;
    
    Acoef   .resize( nField+1 ); rLOOP Acoef[r]   .resize( bandwidth+1 );
    Jacobian.resize( nField+1 ); rLOOP Jacobian[r].resize( bandwidth+1 );
    Jcoef   .resize( nField+1 ); rLOOP Jcoef[r]   .resize( bandwidth+1 );
    b       .resize( nField+1 );
    phi     .resize( nField+1 );
    dPhi    .resize( nField+1 );
    phiNew  .resize( nField+1 );
    minusf  .resize( nField+1 );

    for ( int r = 0 ; r <= nField ; ++r )
      {
	phi   [r] = 0.;
	dPhi  [r] = 0.;
	phiNew[r] = 0.;
	Qval  [r] = 0.1;
      }

    pi = 3.141596;

    myPE = myMPI.myPE;

  }

  //  ==
  //  ||
  //  ||  Nonlinear right-hand-side functions
  //  ||
  //  ==

  double nlRHS(double c0, double tau, double phiVal)
  {
    //    return c0*exp( tau * phiVal );
    return c0*sin( tau * phiVal );
  }
    
  double nlRHS_pd(double c0, double tau, double phiVal)
  {
    //    return tau*c0*exp( tau * phiVal );
    return c0*tau*cos(tau * phiVal);
  }
    
  //  ==
  //  ||
  //  ||  Form Linear System Ax = b
  //  ||
  //  ==

  void FormLS(mpiInfo &myMPI, double c0, double tau)
  {
    double half    = 0.5;
    double quarter = 0.25;
    
    rLOOP cLOOP Acoef   [r][c] = 0.;  // Initialize linear system
    rLOOP cLOOP Jacobian[r][c] = 0.;  // Initialize Jacobian
    rLOOP cLOOP Jcoef   [r][c] = 0 ;  // The default is to point to entry zero; note that phi[0] is set to 0, above.
    rLOOP       Jcoef   [r][1] = r;   //
    rLOOP        b      [r] = 0.;
    
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
	  Acoef[p][1] += half    * ( -1./dx2 - 1./dy2 );
	  Acoef[p][2] += half    *    1./dx2;
	  Acoef[p][4] += half    *    1./dy2;
	  b[p]        += quarter * nlRHS(c0,tau,phi[p]);

	  p = pid(i+1,j);
	  Acoef[p][1] += half    * ( -1./dx2 - 1./dy2 );
	  Acoef[p][3] += half    *    1./dx2;
	  Acoef[p][4] += half    *    1./dy2;
	  b[p]        += quarter * nlRHS(c0,tau,phi[p]);

	  p = pid(i+1,j+1);
	  Acoef[p][1] += half    * ( -1./dx2 - 1./dy2 );
	  Acoef[p][3] += half    *    1./dx2;
	  Acoef[p][5] += half    *    1./dy2;
	  b[p]        += quarter * nlRHS(c0,tau,phi[p]);

	  p = pid(i,j+1);
	  Acoef[p][1] += half    * ( -1./dx2 - 1./dy2 );
	  Acoef[p][2] += half    *    1./dx2;
	  Acoef[p][5] += half    *    1./dy2;
	  b[p]        += quarter * nlRHS(c0,tau,phi[p]);
	  
      }

    // Loop over each cell, and let each cell contribute to its points' Jacobian
    
    for ( int i = 1 ; i <= nStaggeredCellx ; ++i )
      for ( int j = 1 ; j <= nStaggeredCelly ; ++j )
	{
	  int p;

	  p = pid(i,j);
	  Jacobian[p][1] += half    * ( -1./dx2 - 1./dy2 ) - quarter * nlRHS_pd(c0,tau,phi[p]);
	  Jacobian[p][2] += half    *    1./dx2;
	  Jacobian[p][4] += half    *    1./dy2;

	  p = pid(i+1,j);
	  Jacobian[p][1] += half    * ( -1./dx2 - 1./dy2 ) - quarter * nlRHS_pd(c0,tau,phi[p]);
	  Jacobian[p][3] += half    *    1./dx2;
	  Jacobian[p][4] += half    *    1./dy2;

	  p = pid(i+1,j+1);
	  Jacobian[p][1] += half    * ( -1./dx2 - 1./dy2 ) - quarter *  nlRHS_pd(c0,tau,phi[p]);
	  Jacobian[p][3] += half    *    1./dx2;
	  Jacobian[p][5] += half    *    1./dy2;

	  p = pid(i,j+1);
	  Jacobian[p][1] += half    * ( -1./dx2 - 1./dy2 ) - quarter *  nlRHS_pd(c0,tau,phi[p]);;
	  Jacobian[p][2] += half    *    1./dx2;
	  Jacobian[p][5] += half    *    1./dy2;
	  
      }

    // Compute right-hand side, f, for Newton system J*delta = -f.  Note that f = A*phi - b
    
    rowLOOP
      {
	minusf[row] =  b[row];
	colLOOP minusf[row] -= Acoef[row][col] * phi[ Jcoef[row][col] ] ;
      }
    
    // Apply BCs for SA and NR

    for ( int i = 1 ; i <= nRealx ; ++i )
      for ( int j = 1 ; j <= nRealy ; ++j )
	{
	  int p         = pid(i,j);
	  //                                                    SA boundary conditions                NR boundary conditions
	  //                                                    ------------------------------------  -------------------------------------------------------
	  if ( myMPI.iPE == 0            ) if ( i == 1      ) { ApplyBC( Acoef , b, p, BCfunc(i,j) ); ApplyBC( Jacobian , minusf, p,  phi[p] - BCfunc(i,j) ); }
	  if ( myMPI.iPE == myMPI.nPEx-1 ) if ( i == nRealx ) { ApplyBC( Acoef , b, p, BCfunc(i,j) ); ApplyBC( Jacobian , minusf, p,  phi[p] - BCfunc(i,j) ); }
	  if ( myMPI.jPE == 0            ) if ( j == 1      ) { ApplyBC( Acoef , b, p, BCfunc(i,j) ); ApplyBC( Jacobian , minusf, p,  phi[p] - BCfunc(i,j) ); }
	  if ( myMPI.jPE == myMPI.nPEy-1 ) if ( j == nRealy ) { ApplyBC( Acoef , b, p, BCfunc(i,j) ); ApplyBC( Jacobian , minusf, p,  phi[p] - BCfunc(i,j) ); }
	}

    // debugging

    //    if ( printWhat == "printLinear") printLinearSystem();
    //    if ( printWhat == "printNewton") printNewtonSystem();

  }

  double BCfunc(int i, int j)
  {
    int p      = pid(i,j);
    return x[p] + y[p];
    //    return sin(pi*x[p]);
  }

  
  void ApplyBC(VDD &Matrix , VD &RHS , int BCrow,double BCvalue)
  {
    
    cLOOP Matrix[BCrow][c] = 0.; 
    cLOOP Jcoef [BCrow][c] = 0 ;

    Matrix[ BCrow ] [ 1 ] = 1.      ; 
    Jcoef [ BCrow ] [ 1 ] = BCrow   ;
    RHS   [ BCrow ]       = BCvalue ;

    rLOOP
      if (  r != BCrow )
	{
	  cLOOP
	    {
	      if ( Jcoef[r][c] == BCrow )
		{
		  RHS[r] -= Matrix[r][c]*BCvalue;
		  Jcoef[r][c] = 0; Matrix[r][c] = 0.;
		}
	    }
	}
  }

  //  ==
  //  ||
  //  ||  Update nonlinear solution 
  //  ||
  //  ==

  int NR_Phi_Update( double tolerance , double relax)
  {
    int converged = 1;
    
    rLOOP
      {
     	phi[r] = phi[r]*relax + (1.-relax)*( phi[r] + dPhi[r] );
	
     	if ( fabs( dPhi[r] ) > tolerance ) converged = 0;
      }
    return converged;
  }

  int SA_Phi_Update( double tolerance , double relax)
  {
    int converged = 1;
    rLOOP
      {
	if ( fabs( phiNew[r] - phi[r] ) > tolerance ) converged = 0;
	phi[r] = relax*phi[r] + (1.-relax)*phiNew[r] ;
      }
    return converged;
  }

  //  ==
  //  ||
  //  ||  Utility routines
  //  ||
  //  ==

  void printLinearSystem()
  {
    rLOOP
      {
	printf("myPE: %d row = %3d: ",myPE,r);
	cLOOP printf(" | %2d--> %9.2f ",Jcoef[r][c],Acoef[r][c] );
	printf(" |||  b--> %9.2f phi = %9.2f \n",b[r],phi[r] );
      }
  }

  void printNewtonSystem()
  {
    rLOOP
      {
	printf("myPE: %d row = %3d: ",myPE,r);
	cLOOP printf(" | %2d--> %9.2f ",Jcoef[r][c],Jacobian[r][c] );
	printf(" ||| -f--> %9.2f phi = %9.2f \n",minusf[r],phi[r] );
      }
  }

 int pid(int i,int j) { return i + (j-1)*nRealx; }  
                                                       
  #include "plotter.h"
  #include "linearSolver.h"
};


