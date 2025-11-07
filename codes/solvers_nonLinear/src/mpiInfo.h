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
  
  double *commL_s;   // Send
  double *commR_s;
  double *commT_s;
  double *commB_s;
  double *commL_r;   // Receive
  double *commR_r;
  double *commT_r;
  double *commB_r;
  
  MPI_Status  status;
  int         err;
  int         tag;
  MPI_Request request;

  VD peMultiplicity;

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

    nRealx = nCellx;
    nRealy = nCelly;

    // Store and check incoming processor counts
    
    nPEx = _nPEx;
    nPEy = _nPEy;
    
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

    if ( iPE > 0      )	nei_w = myPE - 1    ;
    if ( jPE > 0      )	nei_s = myPE - nPEx ; 
    if ( iPE < nPEx-1 )	nei_e = myPE + 1    ; 
    if ( jPE < nPEy-1 )	nei_n = myPE + nPEx ; 
    
    // Acquire memory for the communication between adjacent processors:

    commL_s = new double [ nRealy+1 ];	    commL_r = new double [ nRealy+1 ];
    commR_s = new double [ nRealy+1 ];	    commR_r = new double [ nRealy+1 ];
    commT_s = new double [ nRealx+1 ];	    commT_r = new double [ nRealx+1 ];
    commB_s = new double [ nRealx+1 ];	    commB_r = new double [ nRealx+1 ];
    
    tag = 0;

    // Set up peMultiplicity

    int nField = nRealx*nRealy;
    peMultiplicity.resize(nField + 1);
    rLOOP peMultiplicity[r] = 1.;
    PEsum(peMultiplicity);
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
  
  void PEsum( VD &field )
  {
    //    return;
    int size_x = nRealx+1;
    int size_y = nRealy+1;
    int tag    = 0;

    // (1) Exchange information in the North-South direction
    
    // (1.1) From the full field "field", extract the boundary values into the comm arrays:

    iLOOP {  commB_s[i] = field[ pid(i,1) ];  commT_s[i] = field[ pid(i,nRealy) ]; }  // top  and bottom

    // (1.2) Send field to the neighboring PEs

    if ( nei_n >= 0 ) err = MPI_Send(commT_s, size_x , MPI_DOUBLE , nei_n , tag , MPI_COMM_WORLD );
    if ( nei_s >= 0 ) err = MPI_Send(commB_s, size_x , MPI_DOUBLE , nei_s , tag , MPI_COMM_WORLD );

    // (1.3) Initialize receiving values at zero.  They will not acquire values unless there is a neighboring PE.

    iLOOP {  commB_r[i] = 0.; commT_r[i] = 0.; }  // top  and bottom

    // (1.4) Receive values from neighboring PEs' physical boundaries.
	
    if ( nei_n >= 0 ) err = MPI_Recv(commT_r, size_x , MPI_DOUBLE , nei_n , tag , MPI_COMM_WORLD , &status);
    if ( nei_s >= 0 ) err = MPI_Recv(commB_r, size_x , MPI_DOUBLE , nei_s , tag , MPI_COMM_WORLD , &status);
	
    // (1.5) Add received values to the "values" array originally provided, along each side.
	
    iLOOP {  field[ pid(i,1) ] += commB_r[i]; field[ pid(i,nRealy) ] += commT_r[i]; }  // top  and bottom

    // (2) Exchange information in the East-West direction (repeat steps, above)

    // (2.1)

    jLOOP{commL_s[j] = field[ pid(1,j) ];	commR_s[j] = field[ pid(nRealx,j) ];      }  // left and right sides

    // (2.2)

    if ( nei_e >= 0 ) err = MPI_Send(commR_s, size_y , MPI_DOUBLE , nei_e , tag , MPI_COMM_WORLD );
    if ( nei_w >= 0 ) err = MPI_Send(commL_s, size_y , MPI_DOUBLE , nei_w , tag , MPI_COMM_WORLD );

    // (2.3)

    jLOOP {  commL_r[j] = 0.; commR_r[j] = 0.; }  // left and right sides

    // (2.4)

    if ( nei_e >= 0 ) err = MPI_Recv(commR_r, size_y , MPI_DOUBLE , nei_e , tag , MPI_COMM_WORLD , &status);
    if ( nei_w >= 0 ) err = MPI_Recv(commL_r, size_y , MPI_DOUBLE , nei_w , tag , MPI_COMM_WORLD , &status);

    // (2.5)

    jLOOP {  field[ pid(1,j) ] += commL_r[j]; field[ pid(nRealx,j) ] += commR_r[j]; }  // left and right sides

  }

  
  int pid(int i,int j) { return i + (j-1)*nRealx; }  

};

