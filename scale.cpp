
/*====================================================================

  THE PROGRAM to scale the data from files.

  ACKNOWLEDGEMENTS:

    Alexey D. Kondorskiy,
    P.N.Lebedev Physical Institute of the Russian Academy of Science.
    E-mail: kondorskiy@lebedev.ru, kondorskiy@gmail.com.


  Last modified: Nov 12, 2020.

====================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <math.h>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <list>
#include <vector>


/*--------------------------------------------------------------------
  Read two column data from file.
--------------------------------------------------------------------*/
bool readTwoColumnData(
  const std::string &name,  // Name of the file to load the data.
  std::vector<double> &x,   // Result vector of arguments.
  std::vector<double> &y)   // Result vector of fcuntion values.
{
  struct stat st;
  if(stat(name.c_str(), &st) != 0)
    return false;
  else {
    std::ifstream fin(name.c_str(), std::ios::in);
    while(!fin.eof()) {
      double tmp;
      fin >> tmp; if(!fin.eof()) x.push_back(tmp);
      fin >> tmp; if(!fin.eof()) y.push_back(tmp);
    }
    fin.close();
    return true;
  }
}


/*--------------------------------------------------------------------
  Subroutine to transform and analyze the data.
--------------------------------------------------------------------*/
void work(std::string inp_file_name, const double &factor)
{
  std::vector<double> x, y;
  if (!readTwoColumnData(inp_file_name, x, y)) return;
  std::string file_name = "scale_" + inp_file_name;
  std::ofstream fout(file_name.c_str(), std::ios::out);
  for(int i = 0; i < y.size(); ++i)
    fout << x[i] << " " << y[i]*factor << "\n";
  fout.close();
}


/*********************************************************************
  Main program.
*********************************************************************/
int main(int argc, char **argv)
{
  double factor = 100.0/0.0172;
  work("line_0.dat", factor);
  work("line_1.dat", factor);
  work("line_2.dat", factor);
  work("line_3.dat", factor);
  work("line_4.dat", factor);
  work("line_5.dat", factor);
  return 0;
}


//====================================================================
