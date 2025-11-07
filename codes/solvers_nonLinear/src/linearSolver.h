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
//  ||  Jacobi
//  ||
//  ||  Performs Jacobi iterations on nField X nField linear
//  ||  system:  A*Solution = RHS
//  ||
//  ==

void Jacobi(VDD &Matrix , VD &RHS , VD &Solution , mpiInfo &myMPI )
  {
    int converged;
    int iter = 0;
    double newval;
    double cur_delta = 0.;
    double tol = 1.e-18;
    int max_iter = nField * 100;

    MPI_Status status;
    int        tag = 0;
    int        err;
    int        global_converged = 0;
    int        zero = 0;
    int        one  = 1;
    
    // (1) Initial guess

    //    rLOOP Solution[r] = 1.;
    
    // (2) Temporary storage for parallel communication
    
    VD Diag_sumPE  ; Diag_sumPE.resize  ( Solution.size() );  // Stores the diagonal for each node
    VD RowSum_sumPE; RowSum_sumPE.resize( Solution.size() );  // Stores RHS - Jacobisum for each node

    rLOOP Diag_sumPE[r] = Matrix[r][1];

    // (3) Sum diagonals on PE boundaries

    myMPI.PEsum( Diag_sumPE );
    
    // (4) Begin Iterations
    
    while ( global_converged == 0 && ++iter <= max_iter )
      {
	// (4.1) Update each row
	
	converged = 1;
	
	rLOOP
	  {
	    RowSum_sumPE[r] = RHS[r];
	    for ( int c = 2 ; c <= bandwidth ; ++c ) RowSum_sumPE[r] -=  Matrix[r][c] * Solution[Jcoef[r][c]];
	  }
	
	myMPI.PEsum( RowSum_sumPE );

	rLOOP
	  {
	    newval = RowSum_sumPE[r]/Diag_sumPE[r];
	    cur_delta  = fabs(Solution[r] - newval);
	    if ( cur_delta > tol ) converged = 0;
	    Solution[r]       = newval;
	  }

	// (4.2) Check convergence across PEs

	MPI_Allreduce(&converged, &global_converged, 1 , MPI_INT, MPI_MIN, MPI_COMM_WORLD);
      }
    
    // (5) Done - inform user

//  if ( global_converged == 1 ) if ( myMPI.myPE == 0 ) cout << "  (o) Jacobi converged in " << iter << " iterations.\n" ;
    if ( global_converged == 0 ) if ( myMPI.myPE == 0 ) cout << "  (o) Jacobi failed to converge after " << iter << " iterations.\n" ;

  }





//  ==//  ||
//  || Utility routine: Dot
//  ||
//  ||
//  || Returns the dot product of vec1 and vec2, where vec1 and vec2 are complete
//  || on each PE.
//  ||
//  ==

double Dot(VD &vec1, VD &vec2 , mpiInfo &myMPI)
{
  double  sum  = 0.;

  // peMulticitgy = 1 except on PE boundaries where it is equal to the
  // number of PEs containing the given node.

  rLOOP sum += vec1[r]*vec2[r] / myMPI.peMultiplicity[r];

  // Sum results across PEs and share that sum with all PEs

  double globalSum;                                                           // ~LabStrip~ 
  MPI_Allreduce(&sum, &globalSum, 1 , MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);   // ~LabStrip~ 

  return globalSum;                                                           // ~LabReplace~ ~globalSum~ ~ TO-DO ~
}


//  ==
//  ||
//  || Utility routine: MatVecProd
//  ||
//  || Computes the matrix-vector product, prod = A*p where p is complete
//  || on each PE but A must be summed on PE boundaries.
//  ||
//  ==

void MatVecProd(VDD &Matrix , VD &p , VD &prod , mpiInfo &myMPI)
{

  // Serial computation on this PE

  rowLOOP
    {
      prod[row] = 0.;
      colLOOP
	{
	  int Acol = Jcoef[row][col];
	  if ( Acol > 0 ) prod[row] += Matrix[row][col] * p[ Acol ];
	}
    }
  
  // Handle PE boundaries

  myMPI.PEsum(prod);                          // ~LabStrip~
}

//  ==
//  ||
//  || Utility routine: Residual
//  ||
//  || Computes the residual, residual = b - A*Sol, where Sol is complete on
//  || each PE but b and A must be summed at PE boundaries.
//  ||
//  ==

void Residual(VDD &Matrix , VD &residual , VD &Sol , VD &RHS, mpiInfo &myMPI)
{

  MatVecProd(Matrix, Sol , residual , myMPI);
  
  rowLOOP residual[row] = RHS[row] - residual[row];
  
}



// ==
// ||
// || CG (Conjugate Gradient)
// ||
// || Solves the system using Conjugate-Gradient iteration.
// ||
// || See https://en.wikipedia.org/wiki/Conjugate_gradient_method
// ||
// ==

void CG(VDD &Matrix , VD &RHS , VD &Solution , mpiInfo & myMPI)
{

  VD rnew; rnew.resize(nField + 1);
  VD r;       r.resize(nField + 1);
  VD p;       p.resize(nField + 1);
  VD Ap;     Ap.resize(nField + 1);
  VD Ax;     Ax.resize(nField + 1);
  
  double p_dot_Ap;         // Stores matrix-vector product
  double r_dot_r;          // Stores r dot r
  double rnew_dot_rnew;    // Stores rnew dot rnew
  double alpha;            // Alpha in the above-referenced algorithm
  double beta;             // Beta   "  "   "       "          "
  double cur_delta;
  double tol           = 1.e-15;
  int global_converged = 0;
  int iter             = 0;
  int max_iter         = nField * 1000;
  int converged;

  // (1) Initial guess and other initializations

  rowLOOP p[row] = r[row] = 0.;
  
  Solution[0] = 0.;
  p       [0] = 0.;
  r       [0] = 0.;

  // (2) Prepare for parallel computations on RHS
  
  VD b_PEsum ;
  b_PEsum.resize(nField + 1 ) ;
  rowLOOP b_PEsum[row] = RHS[row];                // ~LabStrip~
  myMPI.PEsum(b_PEsum);                           // ~LabStrip~
  
  // (3) Initialize residual, r, and r dot r for CG algorithm

  Residual(Matrix , r , Solution,b_PEsum,myMPI);

  rowLOOP p[row] = r[row];

  r_dot_r  = Dot(r,r,myMPI);         
      
  // (4) CG Iterations

  while ( global_converged == 0  && ++iter <= max_iter)
    {
      // (4.1) Compute alpha

      MatVecProd(Matrix,p,Ap,myMPI);      // A*p (stored in Ap)
      p_dot_Ap = Dot(p,Ap,myMPI);         // p*Ap
      alpha    = r_dot_r / p_dot_Ap;

      // (4.2) Update solution and residual

      rowLOOP Solution[row] = Solution[row] + alpha *  p[row];
      rowLOOP rnew    [row] = r[row]        - alpha * Ap[row];

      // (4.3) Compute beta

      rnew_dot_rnew = Dot(rnew,rnew,myMPI);
      beta          = rnew_dot_rnew / r_dot_r;
      
      // (4.4) Update search direction

      rowLOOP p[row] = rnew[row] + beta*p[row];
      
      // (4.5) r "new" will be r "old" for next iteration

      rowLOOP r[row] = rnew[row];
      r_dot_r        = rnew_dot_rnew;

      // (4.6) Check convergence on this PE


      rowLOOP
	{
	  cur_delta  = fabs(alpha * p[row]);
	  if ( cur_delta > tol ) converged = 0;
	}

      if ( fabs(r_dot_r) < tol)
	converged = 1;
      else
	converged = 0;
      
      // (4.7) Check convergence across PEs, store result in "global_converged"

      MPI_Allreduce(&converged, &global_converged, 1 , MPI_INT, MPI_MIN, MPI_COMM_WORLD);   // ~LabStrip~

    }

    // (5) Done - Inform user

//if ( global_converged == 1 ) if ( myMPI.myPE == 0 ) cout << "  (o) CG converged in " << iter << " iterations.\n" ;
  if ( global_converged == 0 ) if ( myMPI.myPE == 0 ) cout << "  (o) CG failed to converge after " << iter << " iterations.\n" ;

}


