//
// main.cpp
//  
// Made by Thac DO
// Login   <dot@Vasa.imag.fr>
// 
// Started on  Thu Apr 1 12:00:00 2010 Thac DO
// Started on  Thu Apr 1 12:00:00 2010 Thac DO
//
#include <fstream>
std::ostream *output; 

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <math.h>

#include "GLCM.hpp"
#include "TransactionSequence.h"
#include "DBMain.h"
#include "BinaryMatrix.h"
#include "TimeTracker.h"

using namespace std;

typedef list<TransactionSequence> LISTTS;
typedef list<int> LISTINT;
typedef GLCM GLCM;

/**
 * Load an ascii file (SPADE sample)
 * File format :
 * 	- first line display items name (must be integers)
 * 	- then each line display items values in the right order
 **/
DBMain * LoadBase (char * fileName) throw ()
{
	ifstream f (fileName);
	string line = "";
	int NbAttr = 1;
	DBMain * mainDB;

	// Looking for the number of attribute
	getline (f, line);
	for (unsigned int i = 0; i < line.length (); i++)
		if (line[i] == ' ')
			NbAttr++;

	// Looking for the number of customers
	int NbCust = 0;
	string LINE = "";
	for (;getline (f, line); NbCust++);

	//mainDB->Initialize (NbCust, NbAttr);
	f.close ();

	// We read again the file, treating its
	ifstream file (fileName);
	line = "";
	getline (file, line); // first line is useless

	vector <string> vecStr;
	string test = "";
	for (unsigned int i = 0; i < line.length (); i++)
	{
		if (line[i] == ' ')
		{
			vecStr.push_back (test);
			test.clear ();
		}
		else
			test +=line[i];
	}
	vecStr.push_back (test);
	int * att = new int [vecStr.size ()];
	for (unsigned int i = 0; i < vecStr.size (); i++)
	{
		att[i] = atoi (vecStr[i].c_str ());
	}

	// Now, loading transaction values
	int nbLine = 0;
	int ** values = new int * [vecStr.size ()];
	for (unsigned int i = 0; i < vecStr.size (); i++)
		values[i] = new int [NbCust];

	LINE = "";
	for (;getline (file, LINE); nbLine++)
	{
		vecStr.clear ();
		string test = "";
		for (unsigned int i = 0; i < LINE.length (); i++)
		{
			if (LINE[i] == ' ')
			{
				vecStr.push_back (test);
				test.clear ();
			}
			else
				test +=LINE[i];
		}
		vecStr.push_back (test);

		for (unsigned int i = 0; i < vecStr.size (); i++)
		{
			values[i][nbLine] = atoi (vecStr[i].c_str ());
		}
		LINE = "";
	}
	mainDB = new DBMain(values,NbCust,NbAttr);
	file.close ();

	cout << "File loaded" << endl;

	for (unsigned int i = 0; i < vecStr.size (); i++)
			delete [] values[i];
	delete [] values;
	delete [] att;
	values = NULL;

	return mainDB;
}

int main(int argc, char** argv){

std::ofstream outputFile; 

  /* Deals with optional argument (ie. output file) */ 
  switch (argc){
  case (4):
    output = &std::cout; 
    cerr<<"No output file, using standard output."<<endl; 
    break;       
  case (5): 
    outputFile.open(argv[4]); 
    if(outputFile.fail()){
      std::cerr<<"Error opening file : "<<argv[4]<<"."<<std::endl;
      exit(EXIT_FAILURE); 
    }
    output = &outputFile; 
    break; 
  default :
    cout << "Usage = " << argv[0] << "<nor/gra> <input file name> <absolute support threshold> <output file>" << endl ;
		cout << "nor : Normal frequent definition" << endl;
		cout << "gra : GRAAK frequent definition" << endl;
		//cout << "y : write to file. All file were stored in OUT folder." << endl;
		//cout << "n : not write to file." << endl;
    exit(EXIT_FAILURE);     
  }
	/*if (argc != 5)  {
		cout << "Usage = " << argv[0] << "<nor/gra> <input file name> <absolute support threshold> <write to file: y/n>" << endl ;
		cout << "nor : Normal frequent definition" << endl;
		cout << "gra : GRAAK frequent definition" << endl;
		cout << "y : write to file. All file were stored in OUT folder." << endl;
		//cout << "n : not write to file." << endl;
	}
	else {*/
		//clock_t start_time, end_time,temps;
		//time_t t1, t2;

		cout<<"\n     -----Debut du programm------       " << endl;
		//t1 = time((time_t *)NULL);

		// Recover arguments
		string freType = argv[1] ;
		char* inputFileName = argv[2] ;
		float threshold = atof(argv[3]) ;
		string so = "n";//argv[4] ;


		DBMain * dbmain = LoadBase(inputFileName);
		//dbmain->PrintInfo();


        TimeTracker tt;
        tt.Start();

		//glcm->setThreshold(threshold);

		/*BinaryMatrix bm = vbm[0];
		bm.PrintInfo();
		bm = bm && vbm[3];
		bm.PrintInfo();
		cout << glcm.CheckFrequent(bm,threshold);

		ItemSet is = glcm.calF(bm,vTS,vbm);
		cout << is << endl;

		BinaryMatrix gBM = glcm.calG(is, vbm);
		gBM.PrintInfo();*/
		GLCM glcm = GLCM();
		glcm.GLCMAlgo(dbmain,threshold,freType,so,*output);
        
        double ts = tt.Stop();
		//glcm->Print_All_Closed_Pattern();
		//

		//delete dbmain;
		//delete glcm;
		//t2 = time((time_t *)NULL);
		cout<<"------------FIN----------"<<endl;
		glcm.PrintNoClosed();
		//cout << "See Transactions with each Itemset above (with note: Transactions begin from 0 --> n" << endl;
		//cout<<"debut   ===>  "<<ctime(&t1)<<endl;
		//cout<<"fin   ===>  "<<ctime(&t2)<<endl;
        cout << "Total: " << setw(5) << ts << " sec" << endl;

        ostringstream ost;
        ost << getpid ();
        string cmd="ps -p "+ost.str ()+" -o rss";
        cout << "Total Memory usage : " << endl;
        system(cmd.c_str());
        delete dbmain;
	//}

	return 0;
}
