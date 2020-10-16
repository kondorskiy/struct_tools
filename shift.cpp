
/*====================================================================

  THE PROGRAM to fit experimantal data.

  ACKNOWLEDGEMENT(S): Alexey D. Kondorskiy,
    P.N.Lebedev Physical Institute of the Russian Academy of Science.
    E-mail: kondor@sci.lebedev.ru, kondorskiy@gmail.com.

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


// Input file ending.
const std::string INPF_END = ".dat";

// Output file prefix.
const std::string OUT_PRE = "shift-";

// Shift.
const double SHIFT = 5.53;


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
  Subroutine to shift the data.
--------------------------------------------------------------------*/
void shiftData(std::string inp_file_name, const double &sft)
{
  std::vector<double> x, y;
  if (!readTwoColumnData(inp_file_name, x, y)) return;
  int n = y.size();

  std::string file_name = OUT_PRE + inp_file_name;
  std::ofstream fout(file_name.c_str(), std::ios::out);
  for(int i = 0; i < n; ++i)
    fout << x[i] + sft << " " << y[i] << "\n";
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
    shiftData(file_list[i], SHIFT);
  }
  return 0;
}


//====================================================================
