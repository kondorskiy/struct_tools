
/*====================================================================

  THE PROGRAM to plot normalized (max = 1) curves from files.

  ACKNOWLEDGEMENT(S): Alexey D. Kondorskiy,
    P.N.Lebedev Physical Institute of the Russian Academy of Science.
    E-mail: kondor@sci.lebedev.ru, kondorskiy@gmail.com.

====================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <complex>
#include <math.h>
#include <iostream>
#include <vector>
#include <sys/stat.h>

using namespace std;


/*********************************************************************
  Basic parameters to setup.
*********************************************************************/

// Number of data files to compare.
const int file_num = 2;

// Names of data files to compare.
const string file_name[] = { "500.dat", "Xpol_extinct_crossect.dat"};


/*********************************************************************
  The Code.
*********************************************************************/

/*--------------------------------------------------------------------
  Read data.
--------------------------------------------------------------------*/
void read(string name, vector<double> &x, vector<double> &y)
{
  x.clear(); y.clear();
  struct stat st;
  if (stat(name.c_str(), &st) != 0) {
    cout << "File " << name << " not found!\n";
    exit(0);
  } else {
    ifstream fin(name.c_str(), ios::in);
    while (!fin.eof()) {
      double tmp;
      fin >> tmp; if(!fin.eof()) x.push_back(tmp);
      fin >> tmp; if(!fin.eof()) y.push_back(tmp);
    }
    fin.close();
  }
}


/*--------------------------------------------------------------------
  Normalize data.
--------------------------------------------------------------------*/
void normalize(vector<double> &y)
{
  double max = 0.0;
  for (int i = 0; i < y.size(); ++i)
    if (y[i] > max) max = y[i];
  if (max == 0.0) return;

  for (int i = 0; i < y.size(); ++i)
    y[i] = y[i]/max;
}


/*********************************************************************
  Main program.
*********************************************************************/
int main(int argc, char **argv)
{
  string plt_name = "plot.plt";
  ofstream fout_p(plt_name.c_str(), ios::out);
  fout_p << "set term png enhanced size 1024,768" << endl;
  fout_p << "set output \"compare.png\"\n";
  fout_p << "plot \\" << endl;

  for (int i = 0; i < file_num; ++i) {

    vector<double> x, y;
    read(file_name[i], x, y);
    normalize(y);

    string out_name = "norm_" + file_name[i];
    ofstream fout_d(out_name.c_str(), ios::out);
    for (int j = 0; j < x.size(); ++j)
      fout_d << x[j] << " " << y[j] << "\n";
    fout_d.close();

    fout_p << "\"" << out_name << "\" u 1:2 w l smooth mcsplines";
    if (i < file_num-1)
      fout_p << ", \\" << endl;
    else
      fout_p << endl;
  }
  fout_p.close();

  string command = "gnuplot " + plt_name;
  system(command.c_str());
  command = "rm " + plt_name;
  system(command.c_str());

  return 0;
};


//====================================================================
