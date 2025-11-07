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

#include "mpi.h"
#include "solvers.h"
#include "mpiInfo.h"
#include "LaplacianOnGrid.h"


//  ==
//  ||
//  ||
//  ||  Main Program
//  ||
//  ||
//  ==

int main(int argc, char *argv[])
{
  
   mpiInfo myMPI;
   MPI_Init     (&argc         , &argv       );
   MPI_Comm_size(MPI_COMM_WORLD, &myMPI.numPE);
   MPI_Comm_rank(MPI_COMM_WORLD,&myMPI.myPE  );


   int nPEx, nPEy, nCellx, nCelly;
   string   solver;
   string nlsolver;
   double relax = 0.3;
   double c0    = 1.;
   double tau   = 1.;
   
   if ( myMPI.myPE == 0 )
     {
       cout << "\n";
       cout << "  +-----------------------------------------------------+\n";
       cout << "  |                                                     |\n";
       cout << "  |              S O L V E R S                          |\n";
       cout << "  |             D E M O   C O D E                       |\n";
       cout << "  |                                                     |\n";
       cout << "  +-----------------------------------------------------+\n";
       cout << "\n";
     }
    
   nPEx = nPEy = 1;  nCellx = nCelly = 10;

   solver   = "none";
   nlsolver = "none";
     
   for (int count =  0 ; count < argc; ++count)
     {
       if ( !strcmp(argv[count],"-nPEx"    ) ) nPEx   = atoi(argv[count+1]);
       if ( !strcmp(argv[count],"-nPEy"    ) ) nPEy   = atoi(argv[count+1]);
       if ( !strcmp(argv[count],"-nCellx"  ) ) nCellx = atoi(argv[count+1]);
       if ( !strcmp(argv[count],"-nCelly"  ) ) nCelly = atoi(argv[count+1]);
       if ( !strcmp(argv[count],"-solver"  ) ) solver =      argv[count+1] ;
       if ( !strcmp(argv[count],"-nl"      ) ) nlsolver =      argv[count+1] ;
       if ( !strcmp(argv[count],"-r"       ) ) relax    = atof(argv[count+1]) ;
       if ( !strcmp(argv[count],"-c0"      ) ) c0       = atof(argv[count+1]) ;
       if ( !strcmp(argv[count],"-tau"     ) ) tau      = atof(argv[count+1]) ;
     }

   if ( myMPI.myPE == 0 )
     {
       cout << "---------------------------------------------------------\n";
       cout << "Input Summary: " << endl;
       cout << "---------------------------------------------------------\n";
       cout << endl;
       cout << "No. PE   in  x-direction: " << nPEx      << endl;
       cout << "             y-direction: " << nPEy      << endl;
       cout << "No. Cells in x-direction: " << nCellx    << endl;
       cout << "             y-direction: " << nCelly    << endl;
       cout << "Linear solver           : " << solver    << endl;
       cout << "Non-Linear solver       : " << nlsolver  << endl;
       cout << "Relaxation parameter    : " << relax     << endl;
       cout << "Leading coefficient, c0 : " << c0        << endl;
       cout << "Nonlinearity param., tau: " << tau       << endl;
       cout << endl;
       cout << "---------------------------------------------------------\n";
       cout << "Execution Begins: " << endl;
       cout << "---------------------------------------------------------\n";
       cout << endl;
     }

       
   myMPI.GridDecomposition(nPEx,nPEy,nCellx,nCelly);

   // -
   // |
   // | Parallel Grid Generation and Laplace Solver
   // |
   // -
   
   double totalLength = 1.;
   double eachPElength_x = totalLength / nPEx;
   double eachPElength_y = totalLength / nPEy;

   double x0 = eachPElength_x * myMPI.iPE;   double x1 = x0 + eachPElength_x;
   double y0 = eachPElength_y * myMPI.jPE;   double y1 = y0 + eachPElength_y;
   
   LaplacianOnGrid MESH(x0,x1,y0,y1,nCellx,nCelly, myMPI );

   MESH.k0 = 1.;
   MESH.k1 = 1.;
   MESH.k2 = 0.;


   // ----------------------------------------------------------------------------
   // Linear Solve
   // ----------------------------------------------------------------------------
   
   if ( myMPI.myPE == 0 ) printf("(o) Performing linear solve for initial guess\n");
      
   MESH.FormLS(myMPI,c0,tau);
      
   if      ( solver == "jacobi" ) MESH.Jacobi(MESH.Acoef,MESH.b,MESH.phi , myMPI );
   else if ( solver == "cg"     ) MESH.CG    (MESH.Acoef,MESH.b,MESH.phi , myMPI );
   else                           FatalError("Solver " + solver + " not found.");
   
   MESH.plot( "phi"  , MESH.phi , 0 , myMPI      );


   // ----------------------------------------------------------------------------
   // Non-Linear Solve
   // ----------------------------------------------------------------------------
   
   if ( myMPI.myPE == 0 ) printf("(o) Nonlinear Solver Begins\n");

   double tol              = 1.e-08;
   int    iter             = 0;
   int    max_iter         = 10000;
   int    converged        = 0;
   int    global_converged = 0;

   // Initial guesses

   for ( int r = 1 ; r <= MESH.nField ; ++r )
     {
       MESH.phiNew[r] = MESH.phi[r];
       MESH.dPhi  [r] = 0.;
     }
   
   // Nonlinear iterations
   
  ANNOTATE_SITE_BEGIN(Nonlinear_iterations);

   while ( global_converged == 0 && ++iter < max_iter )
     {
       ANNOTATE_TASK_BEGIN(Nonlinear_update);
      
       MESH.FormLS(myMPI,c0,tau);

       if ( nlsolver == "nr" )  
	 {
	   
	   if      ( solver == "jacobi" ) MESH.Jacobi(MESH.Jacobian , MESH.minusf , MESH.dPhi  , myMPI );
	   else if ( solver == "cg"     ) MESH.CG    (MESH.Jacobian , MESH.minusf , MESH.dPhi  , myMPI );
	   else                           FatalError("Solver " + solver + " not found.");

	   converged = MESH.NR_Phi_Update( tol , relax );
	 }
       
       else if ( nlsolver == "sa" ) 
	 {
	   
	   if      ( solver == "jacobi" ) MESH.Jacobi(MESH.Acoef , MESH.b , MESH.phiNew , myMPI );
	   else if ( solver == "cg"     ) MESH.CG    (MESH.Acoef , MESH.b , MESH.phiNew , myMPI );	   
	   else                           FatalError("Solver " + solver + " not found.");

	   converged = MESH.SA_Phi_Update ( tol , relax );
	 }
       
       else
	 FatalError("Nonlinear Solver " + nlsolver + " not found.");

       ANNOTATE_TASK_END();
       
       MPI_Barrier(MPI_COMM_WORLD);   MPI_Allreduce(&converged, &global_converged, 1 , MPI_INT, MPI_MIN, MPI_COMM_WORLD);
       
     }
   
   ANNOTATE_SITE_END();

   if ( global_converged == 1 ) if ( myMPI.myPE == 0 ) cout << "(o) " << nlsolver << " converged in " << iter << " iterations.\n" ;
   if ( global_converged == 0 ) if ( myMPI.myPE == 0 ) cout << "(o) " << nlsolver << " failed to converge after " << iter << " iterations.\n" ;
    
   MESH.plot( "phi_"   + nlsolver + "_" + solver , MESH.phi    , 0 , myMPI      );



   
   // -
   // |
   // | Wrap-Up
   // |
   // -
   
   //   myTime.Finish(myMPI.myPE);

   MPI_Finalize();
   return 0;

}
