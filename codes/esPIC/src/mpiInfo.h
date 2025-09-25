

//  ==
//  ||
//  ||   C L A S S:    m p i I n f o
//  ||
//  ==

class mpiInfo
{
 public:

  int myPE;
  int numPE;
  int nRealx,  nRealy;
  int nPEx, nPEy;
  int iPE , jPE;
  int iMin, iMax, jMin, jMax ; // The global i-j numbers on this processor
  int nei_n, nei_s, nei_e, nei_w;
  int nei_ne, nei_nw, nei_se, nei_sw;
  
  int countx, county;

  double *phiSend_n,  *phiSend_s;
  double *phiSend_e,  *phiSend_w;
  double *phiRecv_n,  *phiRecv_s, *phiRecv_e,  *phiRecv_w;
  
  MPI_Status  status;
  int         err;
  int         tag;
  MPI_Request request;

  //  -
  //  |
  //  |   GridDecomposition: Set up PE numbering system in figure below and
  //  |                      establish communication arrays.
  //  |
  //  |                      nPEx -- number of PEs in the x-direction
  //  |                      nPEy -- number of PEs in the y-direction
  //  |                      numPE = total number of PEs
  //  |
  //  |                       +-------+-------+ . . .   +-------+
  //  |                       |       |       |         |       |
  //  |                       |       |       |         | numPE |
  //  |                       |       |       |         |       |
  //  |                       +-------+-------+ . . .   +-------+
  //  |                       .       .       .         .       .
  //  |                       .       .       .         .       .
  //  |                       .       .       .         .       .
  //  |                       +-------+-------+ . . .   +-------+
  //  |                       |       |       |         |       |
  //  |                       | nPEx  | nPEx+1|         |       |
  //  |                       |       |       |         |       |
  //  |                       +-------+-------+ . . .   +-------+
  //  |                       |       |       |         |       |
  //  |                       |   0   |   1   |         | nPEx-1|
  //  |                       |       |       |         |       |
  //  |                       +-------+-------+ . . .   +-------+
  //  |
  //  |
  //  -
  

  void GridDecomposition(int _nPEx, int _nPEy, int nCellx , int nCelly)
  {

    // local grid size
    nRealx = nCellx;
    nRealy = nCelly;

    // Store and check incoming processor counts
    
    // processor grid shape
    nPEx = _nPEx;
    nPEy = _nPEy;
    
    // confirm grid results in allocated processors numPE
    if (nPEx*nPEy != numPE)
      {
    	if ( myPE == 0 ) cout << "Fatal Error:  Number of PEs in x-y directions do not add up to numPE" << endl;
    	MPI_Barrier(MPI_COMM_WORLD);
    	MPI_Finalize();
    	exit(0);
      }
    
    // Get the i-j location of this processor, given its number.  See figure above:
    
    jPE = int(myPE/nPEx);
    iPE = myPE - jPE*nPEx;

    // Set neighbor values

    nei_n = nei_s = nei_e = nei_w = -1;

    if ( iPE > 0      )
      {
	nei_w = myPE - 1    ;
      }
    if ( jPE > 0      )
      {
	nei_s = myPE - nPEx ;  
      }
    if ( iPE < nPEx-1 )
      {
	nei_e = myPE + 1    ;  
      }
    if ( jPE < nPEy-1 )
      {
	nei_n = myPE + nPEx ;  
      }

    nei_nw = nei_sw = nei_ne = nei_se = -1;
    
    if ( iPE > 0      && jPE > 0      )  nei_sw = myPE - 1 - nPEx  ;
    if ( iPE < nPEx-1 && jPE > 0      )  nei_se = myPE + 1 - nPEx  ;
    if ( iPE > 0      && jPE < nPEy-1 )  nei_nw = myPE - 1 + nPEx  ;
    if ( iPE < nPEx-1 && jPE < nPEy-1 )  nei_ne = myPE + 1 + nPEx  ;

    // Acquire memory for the communication between adjacent processors:
    countx = nRealx + 2;
    county = nRealy + 2;
    
    phiSend_n = new double [ countx ];
    phiSend_s = new double [ countx ];
    phiSend_e = new double [ county ];
    phiSend_w = new double [ county ];
    phiRecv_n = new double [ countx ];
    phiRecv_s = new double [ countx ];
    phiRecv_e = new double [ county ];
    phiRecv_w = new double [ county ];

    tag = 0;
  }

  void ExchangeBoundaryInfo(VD &Solution, VD &b)
  {
	sLOOP phiSend_n[s] = 0.;
	sLOOP phiSend_s[s] = 0.;
	tLOOP phiSend_e[t] = 0.;
	tLOOP phiSend_w[t] = 0.;
	
	// ----------------------------------------------
	// Parallel communication on PE Boundaries       ** See fd.h for tLOOP and sLOOP macros **
	// ----------------------------------------------

	// (1.1) Put values into communication arrays
	
	sLOOP phiSend_n[s] = Solution[ pid(    s , nRealy-1 ) ]; 
	sLOOP phiSend_s[s] = Solution[ pid(    s ,    2     ) ]; 
	tLOOP phiSend_w[t] = Solution[ pid(    2 ,    t     ) ];      
	tLOOP phiSend_e[t] = Solution[ pid( nRealx-1 ,    t ) ];

	// (1.2) Send them to neighboring PEs

	if ( nei_n >= 0 )  err = MPI_Isend(phiSend_n, countx , MPI_DOUBLE , nei_n , tag , MPI_COMM_WORLD , &request );
	if ( nei_s >= 0 )  err = MPI_Isend(phiSend_s, countx , MPI_DOUBLE , nei_s , tag , MPI_COMM_WORLD , &request );
	if ( nei_e >= 0 )  err = MPI_Isend(phiSend_e, county , MPI_DOUBLE , nei_e , tag , MPI_COMM_WORLD , &request );
	if ( nei_w >= 0 )  err = MPI_Isend(phiSend_w, county , MPI_DOUBLE , nei_w , tag , MPI_COMM_WORLD , &request );

	// (1.3) Receive values from neighobring PEs' physical boundaries.
	
	if ( nei_n >= 0 ) { err = MPI_Irecv(phiRecv_n, countx , MPI_DOUBLE , nei_n , tag , MPI_COMM_WORLD , &request );   MPI_Wait(&request,&status); } 
	if ( nei_s >= 0 ) { err = MPI_Irecv(phiRecv_s, countx , MPI_DOUBLE , nei_s , tag , MPI_COMM_WORLD , &request );   MPI_Wait(&request,&status); } 
	if ( nei_e >= 0 ) { err = MPI_Irecv(phiRecv_e, county , MPI_DOUBLE , nei_e , tag , MPI_COMM_WORLD , &request );   MPI_Wait(&request,&status); } 
	if ( nei_w >= 0 ) { err = MPI_Irecv(phiRecv_w, county , MPI_DOUBLE , nei_w , tag , MPI_COMM_WORLD , &request );   MPI_Wait(&request,&status); } 
	
	// (1.4) Update BCs using the exchanged information
	
	if ( nei_n >= 0 ) sLOOP b[ pid ( s        , nRealy+1 ) ] = phiRecv_n[s] ;  
	if ( nei_s >= 0 ) sLOOP b[ pid ( s        ,    0     ) ] = phiRecv_s[s] ;  
	if ( nei_e >= 0 ) tLOOP b[ pid ( nRealx+1 ,    t     ) ] = phiRecv_e[t] ;  
	if ( nei_w >= 0 ) tLOOP b[ pid ( 0        ,    t     ) ] = phiRecv_w[t] ;  
  }

  
  //  ==
  //  ||
  //  ||  ParticlesExchange
  //  ||
  //  ||  Exchange particles between processors
  //  ||
  //  ||
  //  ==

    void ParticleExchange( VI &ptcl_send_list , VI &ptcl_send_PE , particles &PTCL)
  {
    
    // (1) Get the max number particles to be send by any particular processor, and make sure all processors  know that number.

    int numToSend = ptcl_send_list.size();      int maxToSend;

    MPI_Allreduce( &numToSend, &maxToSend, 1 , MPI_INT, MPI_MAX, MPI_COMM_WORLD); 

    // (2) Allocate contributions to the upcoming Gather operation.  Here, "C" for "Contribution" to be Gathered

    int    *Cptcl_PE;  Cptcl_PE = new int    [maxToSend];  // Particles' destination PEs 
    double *Cptcl_x ;  Cptcl_x  = new double [maxToSend];
    double *Cptcl_y ;  Cptcl_y  = new double [maxToSend];
    double *Cptcl_vx;  Cptcl_vx = new double [maxToSend];
    double *Cptcl_vy;  Cptcl_vy = new double [maxToSend];

    // (3) Populate contributions on all processors for the upcoming Gather operation

    for ( int i = 0 ; i < maxToSend ; ++i ) { Cptcl_PE[i] = -1; Cptcl_x [i] = 0.; Cptcl_y [i] = 0.; Cptcl_vx[i] = 0.; Cptcl_vy[i] = 0.; }


    // (4) Populate with all the particles on this PE.  Note that some/most processors will have left-over space in the C* arrays.
    
    for ( int i = 0 ; i < ptcl_send_list.size() ; ++i )
      {
	int id      = ptcl_send_list[ i]; 
	Cptcl_PE[i] = ptcl_send_PE  [ i]; 
	Cptcl_x [i] = PTCL.x        [id]; 
	Cptcl_y [i] = PTCL.y        [id]; 
	Cptcl_vx[i] = PTCL.vx       [id]; 
	Cptcl_vy[i] = PTCL.vy       [id]; 
      }

    // (5) Allocate and initialize the arrays for upcoming Gather operation to PE0.  The sizeOfGather takes
    //     into account the number of processors, like this figure:
    //
    //     |<----------------------------- sizeOfGather ------------------------------>|  
    //     |                                                                           |
    //     |                                                                           |
    //     |<- maxToSend    ->|<- maxToSend    ->|<- maxToSend    ->|<- maxToSend    ->| 
    //     +------------------+------------------+------------------+------------------+
    //             PE0               PE1                PE2               PE3           

    int sizeOfGather = maxToSend*numPE;   

    int    *Gptcl_PE;  Gptcl_PE = new int    [sizeOfGather];                                 
    double *Gptcl_x ;  Gptcl_x  = new double [sizeOfGather];
    double *Gptcl_y ;  Gptcl_y  = new double [sizeOfGather];
    double *Gptcl_vx;  Gptcl_vx = new double [sizeOfGather];
    double *Gptcl_vy;  Gptcl_vy = new double [sizeOfGather];
    
    for ( int i = 0 ; i < sizeOfGather ; ++i ) { Gptcl_PE[i] = -1; Gptcl_x [i] = 0.; Gptcl_y [i] = 0.; Gptcl_vx[i] = 0.; Gptcl_vy[i] = 0.;  }

    
    // (6)  Gather "Contributions" ("C" arrays) from all PEs onto all PEs into these bigger arrays so all PE will know what particles
    //      need to go where.
    
    MPI_Barrier(MPI_COMM_WORLD);
    
    MPI_Allgather( Cptcl_PE , maxToSend , MPI_INT   , Gptcl_PE , maxToSend , MPI_INT   ,  MPI_COMM_WORLD); 
    MPI_Allgather( Cptcl_x  , maxToSend , MPI_DOUBLE, Gptcl_x  , maxToSend , MPI_DOUBLE,  MPI_COMM_WORLD); 
    MPI_Allgather( Cptcl_y  , maxToSend , MPI_DOUBLE, Gptcl_y  , maxToSend , MPI_DOUBLE,  MPI_COMM_WORLD); 
    MPI_Allgather( Cptcl_vx , maxToSend , MPI_DOUBLE, Gptcl_vx , maxToSend , MPI_DOUBLE,  MPI_COMM_WORLD); 
    MPI_Allgather( Cptcl_vy , maxToSend , MPI_DOUBLE, Gptcl_vy , maxToSend , MPI_DOUBLE,  MPI_COMM_WORLD); 


    MPI_Barrier(MPI_COMM_WORLD);

    // (7) Put in vector form so they can be added to PTCL.  These arrays are 1-based.

    int Np = 0;  for ( int i = 0 ; i < sizeOfGather ; ++i ) if ( Gptcl_PE[i] == myPE ) ++Np;
    
    VD  std_add_x  ;  std_add_x.resize  ( Np+1 );
    VD  std_add_y  ;  std_add_y.resize  ( Np+1 );
    VD  std_add_vx ;  std_add_vx.resize ( Np+1 );
    VD  std_add_vy ;  std_add_vy.resize ( Np+1 );

    int count = 1;
    for ( int i = 0 ; i < sizeOfGather ; ++i )
      if ( Gptcl_PE[i] == myPE )           
	{
	  std_add_x [count] = Gptcl_x[i];  
	  std_add_y [count] = Gptcl_y [i]; 
	  std_add_vx[count] = Gptcl_vx[i]; 
	  std_add_vy[count] = Gptcl_vy[i]; 
	  ++count;                         
	}

    PTCL.add(std_add_x , std_add_y, std_add_vx , std_add_vy );

    // (8) Free up memory

    if (maxToSend    > 0 ) { delete[] Cptcl_PE;  delete[] Cptcl_x ;  delete[] Cptcl_y ; delete[] Cptcl_vx ; delete[] Cptcl_vy;  }
    if (sizeOfGather > 0 ) { delete[] Gptcl_PE;  delete[] Gptcl_x ;  delete[] Gptcl_y ; delete[] Gptcl_vx ; delete[] Gptcl_vy;  }

  }

  
  //  ==
  //  ||
  //  || Routine PEsum
  //  ||
  //  || This routine receives a field variable named "field"; it has a value for
  //  || each node in the mesh local to this PE.  However, its value on this PE's
  //  || boundary nodes is lacking the contributions from the neighboring processor.
  //  || Here, values in "field" are exchanged between neighboring processors so that
  //  || each processor can add the contributions from the neighboring processor.
  //  ||
  //  ==


// ==
// ||
// || Routine: PEsum
// || Purpose: Sum field values on PE boundaries by exchanging with neighbors
// ||          and adding overlapping node contributions.
// || This makes sure shared nodes at subdomain interfaces are consistent.
// ==
  
  void PEsum( VD &field )
  {
    // Memory arrays to store values to send to neighboors
        iLOOP fieldSend_n[i] = 0. ;
        iLOOP fieldSend_s[i] = 0. ;
        jLOOP fieldSend_e[j] = 0. ;
        jLOOP fieldSend_w[j] = 0. ;

        // for the NORTH and SOUTH use i loops...
        // populate send arrays
        iLOOP fieldSend_n[i] = field[ pid( i , nRealy ) ];
        iLOOP fieldSend_s[i] = field[ pid( i , 1      ) ];

        // send arrays
        if (nei_n >= 0) err = MPI_Isend(fieldSend_n, nRealx, MPI_DOUBLE, nei_n, tag, MPI_COMM_WORLD, &request);
        if (nei_s >= 0) err = MPI_Isend(fieldSend_s, nRealx, MPI_DOUBLE, nei_s, tag, MPI_COMM_WORLD, &request);

        // recieve arrays
        if (nei_n > 0) { err = MPI_Irecv(fieldRecv_n, nRealx, MPI_DOUBLE, nei_n, tag, MPI_COMM_WORLD, &request ); MPI_Wait(&request, &status); }
        if (nei_s > 0) { err = MPI_Irecv(fieldRecv_s, nRealx, MPI_DOUBLE, nei_s, tag, MPI_COMM_WORLD, &request ); MPI_Wait(&request, &status); }

        // sum field values
        iLOOP field[ pid( i , nRealy ) ] += fieldRecv_n[i] ;
        iLOOP field[ pid( i ,	   1 ) ] += fieldRecv_s[i] ;

        // repeat now for EAST and WEST using j loops
        // populate send arrays
        jLOOP fieldSend_e[j] = field[ pid( nRealx , j ) ];
        jLOOP fieldSend_w[j] = field[ pid( 1	  , j ) ];

        // send arrays
        if (nei_e >= 0) err = MPI_Isend(fieldSend_e, nRealy, MPI_DOUBLE, nei_e, tag, MPI_COMM_WORLD, &request);
        if (nei_w >= 0) err = MPI_Isend(fieldSend_w, nRealy, MPI_DOUBLE, nei_w, tag, MPI_COMM_WORLD, &request);

        // recieve arrays
        if (nei_e > 0) { err = MPI_Irecv(fieldRecv_e, nRealy, MPI_DOUBLE, nei_e, tag, MPI_COMM_WORLD, &request ); MPI_Wait(&request, &status); }
        if (nei_w > 0) { err = MPI_Irecv(fieldRecv_w, nRealy, MPI_DOUBLE, nei_w, tag, MPI_COMM_WORLD, &request ); MPI_Wait(&request, &status); }

        // sum field values
        jLOOP field[ pid( nRealx , j ) ] += fieldRecv_e[j] ;
        jLOOP field[ pid( 1	 , j ) ] += fieldRecv_w[j] ;
    }

  // indexing by mapping 2D coordinates (i,j) to 1D array
  // note: nRealx+2 accounts for ghost layers
  int pid(int i,int j) { return (i+1) + (j)*(nRealx+2); }  

};

