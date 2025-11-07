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
   string solver;
   
   //   timingInfo myTime("main");

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
    
    //    myTime.Start(myMPI.myPE);

    solver = "none";
     
   for (int count =  0 ; count < argc; ++count)
     {
       if ( !strcmp(argv[count],"-nPEx"    ) ) nPEx   = atoi(argv[count+1]);
       if ( !strcmp(argv[count],"-nPEy"    ) ) nPEy   = atoi(argv[count+1]);
       if ( !strcmp(argv[count],"-nCellx"  ) ) nCellx = atoi(argv[count+1]);
       if ( !strcmp(argv[count],"-nCelly"  ) ) nCelly = atoi(argv[count+1]);
       if ( !strcmp(argv[count],"-solver"  ) ) solver =      argv[count+1] ;
     }

   if ( solver == "none" )

   if ( myMPI.myPE == 0 )
     {
       cout << endl;
       cout << "Input Summary: " << endl;
       cout << "--------------------------- " << endl;
       cout << "No. PE   in  x-direction: " << nPEx   << endl;
       cout << "             y-direction: " << nPEy   << endl;
       cout << "No. Cells in x-direction: " << nCellx << endl;
       cout << "             y-direction: " << nCelly << endl;
       cout << "Linear solver           : " << solver << endl;
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

   MESH.FormLS(myMPI);  

   if      ( solver == "jacobi" ) MESH.Jacobi(MESH.phi , myMPI );
   else if ( solver == "cg"     ) MESH.CG    (MESH.phi , myMPI );
   else                           FatalError("Solver " + solver + " not found.");

   MESH.plot( "phi"      , MESH.phi     , 0 , myMPI      );
   MESH.plot( "phi_mms"  , MESH.phi_mms , 0 , myMPI      );


   

   MPI_Finalize();
   return 0;

}
