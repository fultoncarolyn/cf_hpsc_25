//  ==
//  ||
//  ||
//  ||  Geometric Utilities
//  ||
//  ||
//  ==

//  ==
//  ||
//  ||  cross: Computes the cross product of v and w, returning the result in c
//  ||
//  ==

void cross( double *v , double *w , double *c )
{
  c[0] = /* TO-DO in Lab */ ;
  c[1] = /* TO-DO in Lab */ ;
  c[2] = /* TO-DO in Lab */ ;
  return;
}
//  ==
//  ||
//  ||  dot: Computes the dot product of v and w
//  ||
//  ==

double dot( double *v , double *w )
{
  return /* TO-DO in Lab */ ;
}


//  ==
//  ||
//  ||  Intersection of a line with a plane.
//  ||
//  ||  n   = vector, normal to the plane
//  ||  p0  = a point in the plane
//  ||  L   = a vector pointing along the line
//  ||  L0  = a point on the line
//  ||
//  ||
//  ||  References
//  ||
//  ||  [1] https://en.wikipedia.org/wiki/Line%E2%80%93plane_intersection
//  ==

int Intersect_LinePlane( double *L0 , double *L , double *p0 , double *n , double *xyzInt )
{

  // Dot the line's vector with the plane's normal:
  
  double dotProduct = /* TO-DO in Lab */ ;

  if ( fabs(dotProduct) < 1.e-10 ) return -1;
  
  // Compute distance from point  L0 to the plane

  double p0L0[3]; kLOOP p0L0[k] = p0[k] - L0[k]; 
  
  double d = dot( p0L0 , n ) / /* TO-DO in Lab */; 

  // Compute the intersection point
  
  xyzInt[0] = /* TO-DO in Lab */ ;
  xyzInt[1] = /* TO-DO in Lab */ ;
  xyzInt[2] = /* TO-DO in Lab */ ;

  return 1;
}



//  ==
//  ||
//  ||  insideCorner: Given a corner of a quad in points A, B, and C, determine
//  ||                if point P is inside that corner.
//  ||
//  ||
//  ||        C        (1) Form vector pointing from A to B: vAB
//  ||         +       (2) Form vector pointing from C to A: vCA
//  ||         |       (3) Form vector pointing from A to P: vAP
//  ||         |       (4) P is inside the C-A-B corner if the cross products
//  ||         |           of these two vectors are aligned, i.e., their dot > 0:
//  ||         |           cA = vAB x vAP  and cB = vCA x vAP
//  ||         |
//  ||        A+------------------+ B
//  ||          \
//  ||           \ 
//  ||            * P
//  ==

bool insideCorner( double *P , double *A, double *B , double *C )
{
  double vAB[3];
  double vCA[3];
  double vAP[3];

  kLOOP
    {
      vAB[k] =  /* TO-DO in Lab */ ;
      vCA[k] =  /* TO-DO in Lab */ ;
      vAP[k] =  /* TO-DO in Lab */ ;
    }

  double cB[3] ; cross( /* TO-DO in Lab */ ); 
  double cC[3] ; cross( /* TO-DO in Lab */ ); 

  if ( dot(cB,cC) >= 0 )
    {
      return true;
    }

  return false;
}

//  ==
//  ||
//  ||  insideQuad: Given a quadrilateral as points in x,y,z arrays
//  ||              determine if point P is inside the quadrilateral.
//  ||    
//  ||              It is inside the quadrilateral if it is inside
//  ||              each of its four corners.
//  ||         
//  ==

int insideQuad( double *Pt , double *Q0 , double *Q1 , double *Q2 , double *Q3 )
{
  if ( insideCorner ( Pt , Q0 , Q1 , Q3 ) )
    if ( insideCorner ( Pt , /* TO-DO in Lab */ )
	 if ( insideCorner ( Pt , /* TO-DO in Lab */ )
	      if ( insideCorner ( Pt , /* TO-DO in Lab */ ) return 1;

  return 0;
}

//  ==
//  ||
//  ||  LineHitsFace: Returns 1 if the line specified by a point L0 and vector L
//  ||                intersect a face specified by four points Q0 - Q3.  
//  ||
//  ==

int LineHitsFace( double *L0 , double *L , double *Q0 , double *Q1 , double *Q2 , double *Q3 , double *n)
{
  // (1) Compute intersection point of line with the plane containing the face

  double xyzInt[3];
  int intersects = Intersect_LinePlane( L0 , L , Q0 , n , xyzInt );  

  if ( ! intersects ) return 0;

  // (2) See if the intersection point (xyzInt) is inside the face's quadrilateral

  return insideQuad( xyzInt , Q0 , Q1, Q2, Q3 );


}
