//  ================================================================================
//  ||                                                                            ||
//  ||              transientDiffusion                                            ||
//  ||              ------------------------------------------------------        ||
//  ||              T R A N S I E N T D I F F U S I O N                           ||
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
#include "transientDiffusion.h"
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
   double tEnd, dt, tPlot;
   string solver;
   int restart = 0;
   
    if ( myMPI.myPE == 0 )
      {
       cout << "\n";
       cout << "------------------------------------------------------\n";
       cout << "\n";
       cout << " S O L V E R S                                        \n";
       cout << " D E M O   C O D E                                    \n";
       cout << "\n";
       cout << " Running on " << myMPI.numPE << " processors \n";
       cout << "\n";
       cout << "------------------------------------------------------\n";
       cout << "\n";
      }

    solver = "none";
     
   for (int count =  0 ; count < argc; ++count)
     {
       if ( !strcmp(argv[count],"-nPEx"    ) ) nPEx    = atoi(argv[count+1]);
       if ( !strcmp(argv[count],"-nPEy"    ) ) nPEy    = atoi(argv[count+1]);
       if ( !strcmp(argv[count],"-nCellx"  ) ) nCellx  = atoi(argv[count+1]);
       if ( !strcmp(argv[count],"-nCelly"  ) ) nCelly  = atoi(argv[count+1]);
       if ( !strcmp(argv[count],"-solver"  ) ) solver  =      argv[count+1] ;
       if ( !strcmp(argv[count],"-tEnd"    ) ) tEnd    = atof(argv[count+1]);
       if ( !strcmp(argv[count],"-dt"      ) ) dt      = atof(argv[count+1]);
       if ( !strcmp(argv[count],"-tPlot"   ) ) tPlot   = atof(argv[count+1]);
       if ( !strcmp(argv[count],"-restart" ) ) restart = 1;                  // <-----  TO-DO:  Make special note of this
     }

   // -
   // |
   // | Echo outputs
   // |
   // -

   if ( myMPI.myPE == 0 )
     {
       cout << endl;
       cout << "Input Summary: " << endl;
       cout << "--------------------------- " << endl;
       cout << "No. PE   in  x-direction: " << nPEx    << endl;
       cout << "             y-direction: " << nPEy    << endl;
       cout << "No. Cells in x-direction: " << nCellx  << endl;
       cout << "             y-direction: " << nCelly  << endl;
       cout << "Linear solver           : " << solver  << endl;
       cout << "End Time                : " << tEnd    << endl;
       cout << "Time Step               : " << dt      << endl;
       cout << "Plot Interval           : " << tPlot   << endl;
       cout << "This is a restart (1/0) : " << restart << endl;               // <-----  TO-DO:  Make special note of this
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

   // -
   // |
   // | Time Marching Loop
   // |
   // -

   double tStart = 0.;

   double timeSinceLastPlot = 0.;
   int    latestIterCount;
   int    count = 0;

   MPI_Barrier(MPI_COMM_WORLD);

   if ( restart )
     {
       MPI_Barrier(MPI_COMM_WORLD);
       MESH.readRestart ( phi_restart.bin ); 
     }

   for ( double time = tStart ; time <= tEnd ; time += dt )
     {

       MPI_Barrier(MPI_COMM_WORLD);

       MESH.FormLS(myMPI, dt);  
       
       MPI_Barrier(MPI_COMM_WORLD);

       if      ( solver == "jacobi" ) MESH.Jacobi(MESH.Acoef , MESH.b , MESH.phiNew , myMPI );
       else if ( solver == "cg"     ) MESH.CG    (MESH.Acoef , MESH.b , MESH.phiNew , myMPI );	   
       else                           FatalError("Solver " + solver + " not found.");
       
       MESH.Transient_UpdatePhi();
       
       // Plot / Restart
 
       timeSinceLastPlot += dt;

       if ( timeSinceLastPlot >= tPlot )
	 {
	   if ( myMPI.myPE == 0 ) cout << "Plotting at time = " << time << " Plot ID = " << count << endl << std::flush;

	   MESH.plot( "phi"  , MESH.phi ,     count , myMPI      );

	   timeSinceLastPlot = 0.;
	   
	   MPI_Barrier(MPI_COMM_WORLD);
	   MESH.writeRestart( phi_restart.bin ); 

	   ++count;
	 }

     }

   if ( myMPI.myPE == 0 ) printf("Execution Completed Successfully\n");

   MPI_Finalize();
   return 0;

}
