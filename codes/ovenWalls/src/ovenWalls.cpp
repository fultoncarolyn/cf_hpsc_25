//  ================================================================================
//  ||                                                                            ||
//  ||              ovenWalls                                                     ||
//  ||              ------------------------------------------------------        ||
//  ||              T H E R M A L   R A D I A T I O N                             ||
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

#include "ovenWalls.h"

class Cylinder
{
public:

  int node;
  int nCella;       // Number of cells in the axial direction
  int nCellc;       // Number of cells in the circular direction
  int nReala;       // Number of nodes in the axial direction
  int nRealc;       // Number of nodes in the circular direction
  int nField;       // Number of nodes
  double dtheta;    // Angular spacing of nodes, in degrees
  double dz;        // Axial spacing of nodes
  double radius;    // Cylinder radius
  double length;    // Cylinder length
  int    **face;    // Faces:  face[f][i] gives the nodes of face f
  int    nFaces;    // Number of faces
  double **coord;   // Vertices
  double **normal;  // Face normals
  double **center;  // Face centers

  Cylinder(){}
  
  Cylinder( int _nCella , int _nCellc , double _radius , double _length , int Out1In2 )
  {
    // Store user inputs
    
    nCella = _nCella;
    nCellc = _nCellc;
    
    nReala = nCella + 1 ;
    nRealc = nCellc     ;
  
    nField = nReala * nRealc; 
    nFaces = nCella * nCellc;

    radius = _radius;
    length = _length;
    
    dtheta = 360.   / nCellc;  
    dz     = length / nCella;

    // Form mesh

    coord  = Array2D_double(nField + 1 , 3);
    face   = Array2D_int   (nFaces + 1 , 5);
    normal = Array2D_double(nFaces + 1 , 3);
    center = Array2D_double(nFaces + 1 , 3);

    int faceCount = 0;
    
    for ( int j = 1 ; j <= nRealc ; ++j )
    for ( int i = 1 ; i <= nReala ; ++i )
      {
	int p = pid(i,j);

	double theta = dtheta * j;
	double zval  = dz     * (i-1);

	coord[p][0] = radius * cos(theta*3.1415/180.);
	coord[p][1] = radius * sin(theta*3.1415/180.);
	coord[p][2] = zval;

	// Form face, i.e., for this face number (which happens to be p)
	// collect the pids of the 4 nodes that comprise this face.

	int point[5];
	point[1] = p    ;
	point[2] = p + 1;
	point[3] = point[2] + nReala;
	point[4] = point[1] + nReala;

	// Correct point for when we have wrapped completely around the circle
	
	if ( j == nRealc )
	  {
	    point[3] = pid(i+1,1);
	    point[4] = pid(i,1);
	  }

	// Store the point values in this face, p (but not for the last point in an i-row

	if ( i < nReala)
	  {
	    ++faceCount;
	    for ( int i = 1 ; i <= 4 ; ++i ) face[faceCount][i] = point[i];
	  }
      }

    // Compute normals for each face
    
    for ( int f = 1 ; f <= nFaces ; ++f )
      {
	double xav = 0. , yav = 0. , zav = 0.;
	for ( int k = 1 ; k <= 4 ; ++k )
	  {
	    xav += coord[ face[f][k] ][0]  ;
	    yav += coord[ face[f][k] ][1]  ;
	    zav += coord[ face[f][k] ][2]  ;
	  }
	
	xav /= 4.;
	yav /= 4.;
	zav /= 4.;
	
	double mag = sqrt((xav*xav + yav*yav));
	
	normal[f][0] = xav/mag;
	normal[f][1] = yav/mag;
	normal[f][2] = 0.;
	center[f][0] = xav;
	center[f][1] = yav;
	center[f][2] = zav;

	if ( Out1In2 == 2 ) for ( int k = 0 ; k < 3 ; ++k ) normal[f][k] *= -1.;

      }
    
  }
    


  #include "plotter.h"
  int pid(int i , int j ) { return ( i + (j-1) * nReala); }

};

#include "geom.h"


//  ==
//  ||
//  ||
//  ||  RayIsBlocked
//  ||
//  ||
//  ==

int RayIsBlocked( Cylinder &cSource      ,   // Mesh from which ray is emanating
		  Cylinder &cBlocker     ,   // Mesh that may block that ray
		  int       sourceFaceID ,   // Emanating face
		  double   *Ray0         ,   // Starting point of ray
		  double   *Ray          ,   // Vector pointing along ray
		  VI &facesBlocking      )   // List of faces that are blocking
{
  double Q0[3], Q1[3], Q2[3], Q3[3];
  double blockerN[3];
  
  int blockerFaceID = 0;
  int blocked = 0;
  
  while ( ! blocked  && ++blockerFaceID <= cBlocker.nFaces )
    {

      // Vertices of potential blocker
      
      kLOOP Q0[k] = cBlocker.coord[ cBlocker.face[blockerFaceID][1] ][k];    
      kLOOP Q1[k] = cBlocker.coord[ cBlocker.face[blockerFaceID][2] ][k];
      kLOOP Q2[k] = cBlocker.coord[ cBlocker.face[blockerFaceID][3] ][k];
      kLOOP Q3[k] = cBlocker.coord[ cBlocker.face[blockerFaceID][4] ][k];

      // Normal of potential blocker
      
      kLOOP blockerN[k] = cBlocker.normal[blockerFaceID][k] ;
      
      //  Look for intersection
      
      blocked = LineHitsFace(Ray0, Ray, Q0, Q1, Q2, Q3, blockerN); 

    }
  
  if ( blocked > 0 ) facesBlocking.push_back(blockerFaceID);

  return blocked;

}


//  ==
//  ||
//  ||
//  ||  Main Program
//  ||
//  ||
//  ==

int main(int argc, char *argv[])
{

  printf("\n");
  printf("\n");
  printf("Ray Tracing Demo Code\n");
  printf("\n");
  printf("\n");
  
  double length = 10.;
  double radius = 10.0;
  //             nCell   nCell                                  Facing In
  //             Axial   Angular     Radius         Length      Or Out
  //            ------  --------    ----------     ----------   -------
  Cylinder CylA(   20  ,  20     ,   radius     ,   length      ,   2      );
  Cylinder CylB(   20  ,  20     ,   radius/2.  ,   length      ,   1      );

  CylA.plot("CylA");
  CylB.plot("CylB");

  // Set up parameters for ray tracing

  VI facesBlocking;
  VI facesHit;

  // R A Y   T R A C E   F O R   S O U R C E   F A C E  1 to target face 100


  double Ray0[3], Ray[3];
  int sourceFaceID = 5;
  int targetFaceID = 51;

  CylA.plotFace("source",sourceFaceID);
  
  // (1) Point on source face
    
  kLOOP Ray0[k] = CylA.center[ sourceFaceID ] [k];

  // (2) Ray to target

  for ( targetFaceID = 1 ; targetFaceID <= CylA.nFaces ; ++targetFaceID )
    {
      kLOOP Ray [k] = CylA.center[ targetFaceID ] [k] - Ray0[k];

      // (3) Plot ray
  
      CylA.plotPointVec("ray", Ray0 , Ray , 1.000 );
  
      // (4) Find blockers of ray

      int blocked = 0;
      
      if ( dot(CylA.normal[sourceFaceID],CylA.normal[targetFaceID]) < .9999  )
	{
	  blocked = RayIsBlocked( CylA , CylB , sourceFaceID , Ray0 , Ray , facesBlocking );
  
	  if ( blocked == 0 ) facesHit.push_back(targetFaceID);
	}
    }

  // Plot hits and blockers
  
  CylA.plotFacesInList("hit",facesHit);
  CylB.plotFacesInList("blockers",facesBlocking);
  
  return 0;

}
