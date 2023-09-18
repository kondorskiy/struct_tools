
/*====================================================================

  THE PROGRAM to cleanup noisy experimental data.

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
#include <vector>
using namespace std;


/***********************************************************************************************************************
  Basic parameters
***********************************************************************************************************************/

// Number of data files to process.
const int file_num = 6;

// Names of data files to process.
const string file_name[] = { "500.dat", "540.dat", "560.dat", "625.dat", "645.dat", "675.dat" };

// Factor to rare data.
const int rare = 15;

// Wavelength range [nm].
const double wI = 375.0;
const double wF = 800.0;
const double wS = 2.0;


/*----------------------------------------------------------------------------------------------------------------------
  Read experimental data.
----------------------------------------------------------------------------------------------------------------------*/
void read_rare(string name, vector<double> &x, vector<double> &y)
{
  struct stat st;
  if (stat(name.c_str(), &st) != 0) {
    cout << "File " << name << " not found!\n";
    exit(0);
  } else {
    ifstream fin;
    fin.open(name.c_str(), ios::in);
    int count = 0;
    while (!fin.eof()) {
      double tmp_x; fin >> tmp_x;
      double tmp_y; fin >> tmp_y;
      if (count % rare == 0) {
        x.push_back(tmp_x);
        y.push_back(tmp_y);
      }
      count++;
    }
    fin.close();
  }
}


/*----------------------------------------------------------------------------------------------------------------------
  Cubic spline interpolation subroutines. Adopted from [W. H. Press, S. A. Teukolsky, W. T. Vetterling
  and B. P. Flannery, "Numerical Recipes in Fortran 77 The Art of Scientific Computing (Vol.1)].
----------------------------------------------------------------------------------------------------------------------*/
void ml_spline(vector<double> x, vector<double> y, double yp1, double ypn, vector<double> &y2)
{
  int n = y.size();
  double p, qn, sig, un;
  vector<double> u(n, 0.0);

  y2[0] = -0.5;
  u[0] = (3.0/(x[1] - x[0]))*((y[1] - y[0])/(x[1] - x[0]) - yp1);
  for (int i = 1; i <= n-2; ++i) {
    sig = (x[i] - x[i-1])/(x[i+1] - x[i-1]);
    p = sig*y2[i-1] + 2.0;
    y2[i] = (sig - 1.0)/p;
    u[i] = (6.0*((y[i+1] - y[i])/(x[i+1] - x[i]) - (y[i] - y[i-1])/(x[i] - x[i-1]))/(x[i+1] - x[i-1]) - sig*u[i-1])/p;
  }
  qn = 0.5;
  un = (3.0/(x[n-1] - x[n-2]))*(ypn - (y[n-1] - y[n-2])/(x[n-1] - x[n-2]));
  y2[n-1] = (un - qn*u[n-2])/(qn*y2[n-2] + 1.0);
  for (int k = n - 2; k >= 0; --k)
    y2[k] = y2[k]*y2[k + 1] + u[k];
}

double ml_splint(vector<double> xa, vector<double> ya, vector<double> y2a, double x)
{
  int n = xa.size();

  int klo = 0;
  int khi = n - 1;
  while ((khi - klo) > 1) {
    int k = (khi + klo)/2;
    if(xa[k] > x)
      khi = k;
    else
      klo = k;
  }
  double h = xa[khi] - xa[klo];
  if (h == 0.0) {
    cout << "bad xa input in ml_splint!\n";
    exit(0);
  }
  double a = (xa[khi] - x)/h;
  double b = (x - xa[klo])/h;
  double y = a*ya[klo] + b*ya[khi] + ((a*a*a - a)*y2a[klo] + (b*b*b - b)*y2a[khi])*h*h/6.0;
  return y;
}


/*----------------------------------------------------------------------------------------------------------------------
  Work.
----------------------------------------------------------------------------------------------------------------------*/
void work(const string &data_file_name)
{
  vector<double> x, y, y2;
  read_rare(data_file_name, x, y);

  int n = y.size();
  double y1 = (y[1] - y[0])/(x[1] - x[0]);
  double yn = (y[n-1] - y[n-2])/(x[n-1] - x[n-2]);
  y2.resize(n);
  ml_spline(x, y, y1, yn, y2);

  int nn = int((wF - wI)/wS) + 1;
  string file_name = "clean-" + data_file_name;
  ofstream fout; fout.open(file_name.c_str(), ios::out);
  for (int i = 0; i < nn; ++i) {
    double w = wI + i*wS;
    fout << w << " " << ml_splint(x, y, y2, w) << "\n";
  }
  fout.close();
}



/***********************************************************************************************************************
  Main program.
***********************************************************************************************************************/
int main(int argc, char **argv)
{
  string plt_name = "plot_noisy_clean.plt";
  ofstream fout_p(plt_name.c_str(), ios::out);
  fout_p << "set term png enhanced size 1024,768" << endl;
  fout_p << "set output \"cleaned.png\"\n";
  // fout_p << "set xrange[375:800]\n";
  fout_p << "plot \\" << endl;

  for (int i = 0; i < file_num; ++i) {
    work(file_name[i]);
    fout_p << "\"" << "clean-" + file_name[i] << "\" u 1:2 w l smooth mcsplines";
    if (i < file_num-1)
      fout_p << ", \\" << endl;
    else
      fout_p << endl;
  }
  fout_p.close();

  string command = "C:\\Soft\\gnuplot\\bin\\gnuplot.exe " + plt_name;
  system(command.c_str());
  command = "rm " + plt_name;
  system(command.c_str());

  return 0;
};


//======================================================================================================================
