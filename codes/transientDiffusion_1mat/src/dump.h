//  ================================================================================
//  ||                                                                            ||
//  ||                R e s t a r t   C a p a b i l i t y                         ||
//  ||                                                                            ||
//  ================================================================================


//  ==
//  ||
//  ||  Write Restart File
//  ||
//  ==

void writeRestart(double &time             ,
		  double &dt               ,
		  double &timeSinceLastPlot,
		  int    &nCellx           ,
		  int    &nCelly           ,
		  int    &count            ,
		  string &solver           ,
		  mpiInfo &myMPI           )
{
   VD headerDbls;  headerDbls.resize(4);   for ( int i = 0 ;  i < headerDbls.size() ; ++i ) headerDbls[i] = 0.;
   VI headerInts;  headerInts.resize(4);   for ( int i = 0 ;  i < headerInts.size() ; ++i ) headerInts[i] = 0 ;

   // ----------------------------------------------------------------------------------------------------
   // Store simulation setup variables and a few scalar state variables into arrays for the dump:
   // ----------------------------------------------------------------------------------------------------
   
   // (A) Double-precision variables to be saved in the dump:
   //
   //    [0] Current simulation time
   //    [1] Time step
   //    [2] Time since last plot was written
   
   headerDbls[0] = time;
   headerDbls[1] = /* TO-DO in Lab */  ;
   headerDbls[2] = /* TO-DO in Lab */  ;

   // (B) Integers variables to be saved in the dump:
   //
   //    [0] Total number of cells (globally) in the x-direction for the entire physical problem
   //    [1] Same for y-direction
   //    [2] An integer reflection the choice of linear solver
   //    [3] The number of plots files already written
   
   headerInts[0] = nCellx * myMPI.nPEx;  
   headerInts[1] = /* TO-DO in Lab */; 
   
   if ( solver == "jacobi" ) headerInts[2] = 1;   
   if ( solver == "cg"     ) headerInts[2] = 2;   

   headerInts[3] = /* TO-DO in Lab */;
   
   // Write the dump

   write_mpiio_dump(/* TO-DO in Lab */  );    
}



//  ==
//  ||
//  ||  Read Restart File
//  ||
//  ==

void readRestart(double &time             ,
		 double &dt               ,
		 double &timeSinceLastPlot,
		 int    &nCellx           ,
		 int    &nCelly           ,
		 int    &count            ,
		 string &solver           ,
		 mpiInfo &myMPI           )
{
   VD headerDbls;  headerDbls.resize(4);   for ( int i = 0 ;  i < headerDbls.size() ; ++i ) headerDbls[i] = 0.;
   VI headerInts;  headerInts.resize(4);   for ( int i = 0 ;  i < headerInts.size() ; ++i ) headerInts[i] = 0 ;

   // Read the dump

   read_mpiio_dump( /* TO-DO in Lab */ );  

   // ----------------------------------------------------------------------------------------------------
   // Retrieve simulation setup variables and a few scalar state variables from arrays
   // ----------------------------------------------------------------------------------------------------
   
   // (A) Double-precision variables to be saved in the dump:
   //
   //    [0] Current simulation time
   //    [1] Time step
   //    [2] Time since last plot was written

   time              = /* TO-DO in Lab */;
   dt                = /* TO-DO in Lab */;
   timeSinceLastPlot = /* TO-DO in Lab */;

   
   // (B) Integers variables to be saved in the dump:
   //
   //    [0] Total number of cells (globally) in the x-direction for the entire physical problem
   //    [1] Same for y-direction
   //    [2] An integer reflection the choice of linear solver
   //    [3] The number of plots files already written
   
   nCellx = headerInts[0] / myMPI.nPEx         ; 
   nCelly = /* TO-DO in Lab */ ;
   
   if ( headerInts[2] == 1 ) solver = "jacobi" ; 
   if ( headerInts[2] == 2 ) solver = "cg"     ;
   
   count  = headerInts[3] ;                      
}



//  ================================================================================
//  ||                                                                            ||
//  ||  U t i l i t y :     S i n g l e - V a l u e   I / O   R o u t i n e s     ||
//  ||                                                                            ||
//  ================================================================================



void write_mpiio_int   ( MPI_File fh ,   int  &val, MPI_Offset &offset ){ MPI_Status status; MPI_File_write_at(fh, offset, &val, 1, MPI_INT   , &status); offset += sizeof(int)   ;}
void write_mpiio_double( MPI_File fh , double &val, MPI_Offset &offset ){ MPI_Status status; MPI_File_write_at(fh, offset, &val, 1, MPI_DOUBLE, &status); offset += sizeof(double);}
void read_mpiio_int    ( MPI_File fh ,   int  &val, MPI_Offset &offset ){ MPI_Status status; MPI_File_read_at (fh, offset, &val, 1, MPI_INT   , &status); offset += sizeof(int)   ;}
void read_mpiio_double ( MPI_File fh , double &val, MPI_Offset &offset ){ MPI_Status status; MPI_File_read_at (fh, offset, &val, 1, MPI_DOUBLE, &status); offset += sizeof(double);}



//  ================================================================================
//  ||                                                                            ||
//  ||  M P I I O   D u m p   W r i t e r                                         ||
//  ||                                                                            ||
//  ================================================================================

void write_mpiio_dump(VD &headerDbls , VI &headerInts , mpiInfo &myMPI)
{
  int myPE    = myMPI.myPE;
  int iPE     = myMPI.iPE;
  int jPE     = myMPI.jPE;
  int nPEx    = myMPI.nPEx;
  int nPEy    = myMPI.nPEy;
  int numPE   = myMPI.numPE;

  // -
  // |
  // | Open the file to be written
  // |
  // -
  
  MPI_File fh;
  MPI_File_open(MPI_COMM_WORLD, "phi_dump.bin", MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fh);

  // -
  // |
  // | Write header information (mostly, simulation setup variables)
  // |
  // -
  
  MPI_Offset offset = 0; 
  MPI_Status  status;
  
  if ( myPE == 0 ) 
    {
      for ( int i = 0 ; i < headerDbls.size() ; ++i ) write_mpiio_double( fh , /* TO-DO in Lab */ );  
      for ( int i = 0 ; i < headerInts.size() ; ++i ) write_mpiio_int   ( fh ,  /* TO-DO in Lab */);  
    }
  MPI_Bcast( &offset , 1 , MPI_INT , 0 ,  MPI_COMM_WORLD);
  MPI_Barrier(MPI_COMM_WORLD);
    
  // -
  // |
  // | Allocate and populate A with phi
  // |
  // -

float **A = Array2D_float(nRealx+1, nRealy+1);
  iLOOP
    jLOOP
  {
    int p = pid(i,j);
    A[i][j] = phi[p];
  }
  
  // -
  // |
  // | Create MPI derived data type that will be used to represent the real nodes in the grid.
  // |
  // -
  
  MPI_Datatype myRealNodes;

  int idxStartThisPE  [2] = { 1        , 1          };  // Index coordinates of the sub-array inside this PE's array, A
  int AsizeThisPE     [2] = { nRealx+1 , nRealy+1   };  // Size of the A array on this PE    
  int sub_AsizeThisPE [2] = { nRealx-1 , nRealy-1   };  // Size of the A-sub-array on this PE
  if ( iPE == nPEx-1 ) { ++sub_AsizeThisPE[0];   }
  if ( jPE == nPEy-1 ) { ++sub_AsizeThisPE[1];   }
  
  // Create and commit
  
  MPI_Type_create_subarray(2, AsizeThisPE, sub_AsizeThisPE, idxStartThisPE, MPI_ORDER_C, MPI_FLOAT, &myRealNodes);
  MPI_Type_commit(&myRealNodes);

  // -
  // |
  // | Create a second derived type that will be used to describe the whole array.
  // |
  // -
  
  MPI_Datatype myPartOfGlobal;
  int idxStartInGlobal [2] = { iPE*(nRealx-1)   , jPE*(nRealy-1)   };  // Index cordinates of the sub-array inside the global array
                                                                       // Note that the global array, which does not exist on any
                                                                       // one PE is zero based.  Thus, for example, PE0, which is
                                                                       // wanting to write its first (1-based) entry A[1][1] to the
                                                                       // disk will be writing the [0][0] entry of the abstract
                                                                       // global array.
  int AsizeGlobal      [2] = { nPEx*(nRealx-1)+1,nPEy*(nRealy-1)+1 }; // Size of the global array

  // Create and commit
  
  MPI_Type_create_subarray(2, AsizeGlobal, sub_AsizeThisPE, idxStartInGlobal, MPI_ORDER_C, MPI_FLOAT, &myPartOfGlobal);
  MPI_Type_commit(&myPartOfGlobal);

  // -
  // |
  // | Set the "view" of the file from this PE's perspective, i.e., where and how this PE should write data
  // |
  // -
  
  MPI_File_set_view (fh, offset, MPI_FLOAT, myPartOfGlobal, "native", MPI_INFO_NULL);

  // -
  // |
  // | Perform the collective write operation and clean up
  // |
  // -

  MPI_File_write_all(fh, &A[0][0], 1, myRealNodes, MPI_STATUS_IGNORE);
  
  MPI_File_close(&fh);
  free(A[0]);
  free(A);
  
  MPI_Type_free(&myPartOfGlobal);
  MPI_Type_free(&myRealNodes);

  
}


//  ================================================================================
//  ||                                                                            ||
//  ||  M P I I O   D u m p   R e a d e r                                         ||
//  ||                                                                            ||
//  ================================================================================

void read_mpiio_dump(VD &headerDbls , VI &headerInts , mpiInfo &myMPI)
{
  int myPE    = myMPI.myPE;
  int iPE     = myMPI.iPE;
  int jPE     = myMPI.jPE;
  int nPEx    = myMPI.nPEx;
  int nPEy    = myMPI.nPEy;
  int numPE   = myMPI.numPE;
  
  int nTotalx = nRealx + 2;
  int nTotaly = nRealy + 2;

  // -
  // |
  // |  Open the file to be read
  // |
  // -
  
  MPI_File fh;
  MPI_File_open(MPI_COMM_WORLD, "phi_dump.bin", MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);

  // -
  // |
  // |  Read header information
  // |
  // -
  
    MPI_Offset offset = 0; 
    MPI_Status  status;
    
    for ( int i = 0 ; i < headerDbls.size() ; ++i ) read_mpiio_double( fh ,  /* TO-DO in Lab */ ); 
    for ( int i = 0 ; i < headerInts.size() ; ++i ) read_mpiio_int   ( fh ,  /* TO-DO in Lab */ ); 
  
    MPI_Barrier(MPI_COMM_WORLD);

  // -
  // |
  // | Allocate and populate A with default values; A will be used to read phi
  // |
  // -
  
  float **A = Array2D_float(nTotalx, nTotaly);
  iLOOP jLOOP { A[i][j] = 0.; }
  
  // -
  // |
  // | Create MPI derived data type that will be used to represent the real nodes in the grid.
  // |
  // -
  
  MPI_Datatype myRealNodes;
  int idxStartThisPE  [2] = { 1        , 1        };  // Index coordinates of the sub-array inside this PE's array, A
  int AsizeThisPE     [2] = { nTotalx  , nTotaly  };  // Size of the A array on this PE    
  int sub_AsizeThisPE [2] = { nRealx   , nRealy   };  // Size of the A-sub-array on this PE

  MPI_Type_create_subarray(2, AsizeThisPE, sub_AsizeThisPE, idxStartThisPE, MPI_ORDER_C, MPI_FLOAT, &myRealNodes);
  MPI_Type_commit(&myRealNodes);

  // -
  // |
  // | Create a second derived type that will be used to describe the whole array.
  // |
  // -
  
  MPI_Datatype myPartOfGlobal;
  int idxStartInGlobal [2] = { iPE * nRealx  , jPE * nRealy  };  // Index coordinates of the sub-array inside the global array
  int AsizeGlobal      [2] = { nPEx*(nRealx-1)+1 , nPEy*(nRealy-1)+1 };  // Size of the global array

  // Adjust start index if not at the left/bottom edge

  if ( iPE > 0 ) idxStartInGlobal[0] = iPE*(nRealx-1);
  if ( jPE > 0 ) idxStartInGlobal[1] = jPE*(nRealy-1);
  
  // Create and commit

  MPI_Type_create_subarray(2, AsizeGlobal, sub_AsizeThisPE, idxStartInGlobal, MPI_ORDER_C, MPI_FLOAT, &myPartOfGlobal);
  MPI_Type_commit(&myPartOfGlobal);
  
  // -
  // |
  // | Set the "view" of the file from this PE's perspective, i.e., where and how this PE should write data
  // |
  // -
  
  MPI_File_set_view (fh, offset, MPI_FLOAT, myPartOfGlobal, "native", MPI_INFO_NULL);

  // -
  // |
  // | Perform the collective read operation and clean up
  // |
  // -

  MPI_File_read_all(fh, &A[0][0], 1, myRealNodes, MPI_STATUS_IGNORE);

  // Store into phi

  iLOOP jLOOP { int p = pid(i,j);  phi[p] = A[i][j] ;  }
  
  // Cleanup 
  
  MPI_File_close(&fh);
  free(A[0]);
  free(A);
  
  MPI_Type_free(&myPartOfGlobal);
  MPI_Type_free(&myRealNodes);

}




