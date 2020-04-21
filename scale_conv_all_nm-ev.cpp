
/*====================================================================

  THE PROGRAM to scale the data from all files with certain
    extension and, optionally, convert from eV to nm or from nm to eV.

  ACKNOWLEDGEMENTS:

    Alexey D. Kondorskiy,
    P.N.Lebedev Physical Institute of the Russian Academy of Science.
    E-mail: kondorskiy@lebedev.ru, kondorskiy@gmail.com.


  Last modified: April 21, 2020.

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
  Parameters.
--------------------------------------------------------------------*/

// Input file ending.
const std::string INPF_END = ".dat";

// Output file prefix.
const std::string OUT_PRE = "scale-";

// Scale factor.
const double FACTOR = 1.0E+10;

/* Parameter to control eV <-> nm conversion:
    0 : no conversion;
    1 : converts eV -> nm;
    2 : converts nm -> eV. */
const int CONV = 0;


/*--------------------------------------------------------------------
  Get maximal value of the vector.
--------------------------------------------------------------------*/
double getMax(std::vector<double> &data)
{
  double max = 0.0;
  for (int i = 0; i < data.size(); ++i)
    if (max < data[i]) max = data[i];
  return max;
}


/*--------------------------------------------------------------------
  Get list of files in the current directory with certain ending.
--------------------------------------------------------------------*/
void getFilesInCurrDirectory(
  std::vector<std::string> &out,  // Result list of file paths.
  const std::string &ending)      // File ending required.
{
  DIR *dir;
  class dirent *ent;
  class stat st;
  out.clear();
  dir = opendir(".");
  while ((ent = readdir(dir)) != NULL) {
    std::string file_name = ent->d_name;
    if (file_name[0] == '.') continue;
    if (stat(file_name.c_str(), &st) == -1) continue;
    if ((st.st_mode & S_IFDIR) != 0) continue;
    if(file_name.find(ending.c_str()) == std::string::npos) continue;
    out.push_back(file_name);
  }
  closedir(dir);
}


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
  Convert eV to nm.
--------------------------------------------------------------------*/
double eV2nm(double eV)
{
  double nu = eV/4.135667e-15;
  double lam = 299792458.0/nu;
  return lam*1.0e9;
}


/*--------------------------------------------------------------------
  Convert nm to eV.
--------------------------------------------------------------------*/
double nm2eV(double nm)
{
  double lam = nm*1.0e-9;
  double nu = 299792458.0/lam;
  return nu*4.135667e-15;
}


/*--------------------------------------------------------------------
  Subroutine to transform and analyze the data.
--------------------------------------------------------------------*/
void work(std::string inp_file_name, const double &factor)
{
  std::vector<double> x, y;
  if (!readTwoColumnData(inp_file_name, x, y)) return;
  int n = y.size();

  std::string file_name = OUT_PRE + inp_file_name;
  std::ofstream fout(file_name.c_str(), std::ios::out);
  if (CONV == 0)
    for(int i = 0; i < n; ++i)
      fout << x[i] << " " << y[i]*factor << "\n";
  if (CONV == 1)
    for(int i = n - 1; i >= 0; --i)
      fout << eV2nm(x[i]) << " " << y[i]*factor << "\n";
  if (CONV == 2)
    for(int i = n - 1; i >= 0; --i)
      fout << nm2eV(x[i]) << " " << y[i]*factor << "\n";
  fout.close();
}


/*********************************************************************
  Main program.
*********************************************************************/
int main(int argc, char **argv)
{
  std::vector<std::string> file_list;
  getFilesInCurrDirectory(file_list, INPF_END);
  int nfile = file_list.size();
  for(int i = 0; i < nfile; ++i) {
    std::cout << "working on \'" << file_list[i] << "\'\n";
    work(file_list[i], FACTOR);
  }
  return 0;
}


//====================================================================
