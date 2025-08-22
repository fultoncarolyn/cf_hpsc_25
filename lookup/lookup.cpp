#include <iostream>

using std :: cout;
using std :: endl;

// ==
// ||
// ||   L O O K U P V A L
// ||
// ||   This routine performs linear interpolation on an x-y table
// ||   of values.
// ||
// ==

double lookupVal(int n, double *x, double *y, double xval)
{

  // x, y   = the table for looking up values.
  // n      = the length of x and y.
  // xval   = the value, provided by the calling routine,
  //          at which we want to interpolate the x-y table.
  //
  // This routine returns the result as a double precision value.
  // There are no safety nets for this code.  If xval is not in
  // the range of the values in x[], this routine simply returns
  // a zero.
  
  for ( int i = 0 ; i < n ; ++i)
      if ( xval >= x[i] && xval <= x[i+1] )
	  return y[i] + (xval - x[i]) * (y[i+1]-y[i]) / (x[i+1]-x[i]);
  
  return 0.;
}


// ==
// ||
// ||   M A I N   R O U T I N E
// ||
// ==

int main(int argc, char *argv[])
{

  // Establish two arrays, x and y, of length 100.

  int n = 100;
  double x[n], y[n];

  // Populate those two arrays to represent a table containing
  // data points cooresponding to the algebraic function
  // y = x^2.

  for ( int i = 0 ; i < n ; ++i)
    {
      x[i] = i;
      y[i] = i*i;
    }

  // Look up the value of the y = x^2 table for the value
  // x = 2.5.

  double xval = 2.5;  double yval = lookupVal(n,x,y,xval);

  // Print the results to "standard out", or "std out".

  cout << "For xval = " << xval << " yval = " << yval << endl;

  return 0;
  
}
