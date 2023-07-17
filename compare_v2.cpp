
/*======================================================================================================================

  THE PROGRAM to plot normalized (max ~ 1) curves from files.
	
  ACKNOWLEDGEMENT(S): Alexey D. Kondorskiy, P.N.Lebedev Physical Institute of the Russian Academy of Science.
    E-mail: kondorskiy@lebedev.ru, kondorskiy@gmail.com.


======================================================================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <sys/stat.h>


// ===== Parameters ====================================================================================================

// Files with data to compare
const int data_file_num = 2;
const std::string data_file_name[] = {  "18nm-Bare-Exp.dat",
                                        "avr2d_extinct_crossect.dat"};

// Wavelength range to plot 
const double plot_wl_min = 450.0;
const double plot_wl_max = 750.0;

// Wavelength range to find maxima to match
const double srch_wl_min = 550.0;
const double srch_wl_max = 700.0;


// ----- Read two column data from file --------------------------------------------------------------------------------
bool readTwoColumnData(
  const std::string &name,                  // Name of the file to load the data.
  std::vector<double> &x,                   // Result std::vector of arguments.
  std::vector<double> &y)                   // Result std::vector of function values.
{
  x.clear(); y.clear();
  struct stat st;
  if (stat(name.c_str(), &st) != 0)
    return false;
  else {
    std::ifstream fin(name.c_str(), std::ios::in);
    while (!fin.eof()) {
      double tmp;
      fin >> tmp; if(!fin.eof()) x.push_back(tmp);
      fin >> tmp; if(!fin.eof()) y.push_back(tmp);
    }
    fin.close();
    return true;
  }
}


// ----- Get maximal value of vector of positive values 'y' within the range [x_min, x_max] ----------------------------
double getMax(const double &x_min, const double &x_max, const std::vector<double> &x, const std::vector<double> &y)
{
  double res = 0.0;
  for (int i = 0; i < x.size(); ++i)
    if ( (x_min <= x[i]) && (x[i] <= x_max) && (res < y[i]) ) res = y[i];
  return res;
}


//**********************************************************************************************************************
int main(void)
{
  std::vector<double> x, y;
  std::string file_name;
  std::ofstream fout;

  for (int i = 0; i < data_file_num; ++i) {

    readTwoColumnData(data_file_name[i], x, y);
    double tmp = getMax(srch_wl_min, srch_wl_max, x, y);
    if (tmp <= 0.0) { std::cout << "No maxima found in file " << data_file_name[i] << std::endl; exit(0); }
    tmp = 1.0/tmp;

    file_name = "scale-" + data_file_name[i];
    fout.open(file_name.c_str(), std::ios::out);
    for (int j = 0; j < x.size(); ++j)
      fout << x[j] << " " << tmp*y[j] << std::endl;
    fout.close();
  }

  file_name = "compare.plt";
  fout.open(file_name.c_str(), std::ios::out);
  fout << "set term png enhanced size 1024,768" << std::endl;
  fout << "set output \"compare.png\"" << std::endl;
  fout << "set xrange[" << plot_wl_min << " : " << plot_wl_max << "]" << std::endl;
  fout << "set grid xtics ytics mxtics mytics" << std::endl;
  fout << "set mxtics 2" << std::endl;
  fout << "set mytics 2" << std::endl;
  fout << "set grid" << std::endl;
  fout << "plot \\" << std::endl;
  for (int i = 0; i < data_file_num; ++i) {
    fout << "\"scale-" << data_file_name[i] << "\" u 1:2 w l lw 3 smooth csplines";
    if (i < (data_file_num-1)) fout << ", \\" << std::endl;
  }
  fout.close();

  std::string command = "C:\\Soft\\gnuplot\\bin\\gnuplot.exe " + file_name;
  // std::string command = "gnuplot " + file_name;
  system(command.c_str());
  // command = "rm " + file_name;
  // system(command.c_str());

  return 0;
}


//======================================================================================================================
