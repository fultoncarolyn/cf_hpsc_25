
void plot(string descriptor)
  {

    string filename = descriptor + ".plt";

    // Open plot file
    
    std::fstream file;
    file.open(filename.c_str(),std::ios::out);

    // Write to plot file

    int p[6];
    
    for ( int f = 1 ; f <= nFaces; ++f )
      //    for ( int f = 1 ; f <= 1; ++f )
      {
	p[1] = face[f][1];
	p[2] = face[f][2];
	p[3] = face[f][3];
	p[4] = face[f][4];
	p[5] = face[f][1];

	for ( int k = 1 ; k <= 5; ++k )
	  file << coord[p[k]][0] << " " << coord[p[k]][1] << " "  << coord[p[k]][2] << endl;
	
	file << endl;
	file << coord[p[1]][0] << " " << coord[p[1]][1] << " "  << coord[p[1]][2] << endl;
	file << endl;
      }

    file.close();
  }



void plotFacesInList(string descriptor, VI &facesToPlot)
  {

    string filename = descriptor + ".plt";

    // Open plot file
    
    std::fstream file;
    file.open(filename.c_str(),std::ios::out);

    // Write to plot file

    int p[6];
	   
    for ( int i = 0 ; i < facesToPlot.size() ; ++i )
      {
	p[1] = face[ facesToPlot[i] ][1];
	p[2] = face[ facesToPlot[i] ][2];
	p[3] = face[ facesToPlot[i] ][3];
	p[4] = face[ facesToPlot[i] ][4];
	p[5] = face[ facesToPlot[i] ][1];

	for ( int k = 1 ; k <= 5; ++k )
	  file << coord[p[k]][0] << " " << coord[p[k]][1] << " "  << coord[p[k]][2] << endl;

	file << endl;
	file << coord[p[1]][0] << " " << coord[p[1]][1] << " "  << coord[p[1]][2] << endl;
	file << endl;
      }

    file.close();
  }


void plotFace(string descriptor, int FaceID )
  {

    string filename = descriptor + ".plt";

    // Open plot file
    
    std::fstream file;
    file.open(filename.c_str(),std::ios::out);

    // Write to plot file

    int p[6];
	   
    p[1] = face[ FaceID ][1];
    p[2] = face[ FaceID ][2];
    p[3] = face[ FaceID ][3];
    p[4] = face[ FaceID ][4];
    p[5] = face[ FaceID ][1];
    
    for ( int k = 1 ; k <= 5; ++k )
      file << coord[p[k]][0] << " " << coord[p[k]][1] << " "  << coord[p[k]][2] << endl;
    
    file << endl;
    file << coord[p[1]][0] << " " << coord[p[1]][1] << " "  << coord[p[1]][2] << endl;
    file << endl;

    file.close();
  }





void plotPointVec(string descriptor, double *p , double *v , double scale)
  {

    string filename = descriptor + ".plt";

    // Open plot file
    
    std::fstream file;
    file.open(filename.c_str(),std::ios::out);

    for ( int k = 0 ; k < 3 ; ++k ) file << p[k]              << " "               ; file << endl;
    for ( int k = 0 ; k < 3 ; ++k ) file << p[k] + v[k]*scale << " "               ; file << endl;

    file.close();
  }


void plotPoint(string descriptor, double *p)
  {
    string filename = descriptor + ".plt";

    // Open plot file
    
    std::fstream file;
    file.open(filename.c_str(),std::ios::out);

    for ( int k = 0 ; k < 3 ; ++k ) file << p[k]              << " "               ; file << endl;

    file.close();
  }


