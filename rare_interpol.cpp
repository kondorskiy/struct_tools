
/*======================================================================================================================

  THE PROGRAM to rare and interpolate experimental data to smooth noise.

  ACKNOWLEDGEMENT(S): Alexey D. Kondorskiy, P.N.Lebedev Physical Institute of the Russian Academy of Science.
    E-mail: kondor@sci.lebedev.ru, kondorskiy@gmail.com.

======================================================================================================================*/

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

const int data_file_num = 2;

// Name of file to rate and interpolate data
const string data_file_name[] = {"11nm-Bare-Exp.dat", "11nm-Bare-Num.dat" };

// Step to read the data to rare
const int data_step = 10;

// Prefix for output file
const string res_pre_name = "smooth-";

// Wavelength range [nm]
const double wI = 450.0;
const double wF = 750.0;
const double wS = 1.0;



/*----------------------------------------------------------------------------------------------------------------------
  Read experimental data rare with integer step of i_step.
----------------------------------------------------------------------------------------------------------------------*/
void read_rare(const string &name, vector<double> &x, vector<double> &y, const int &i_step)
{
  struct stat st;
  if (stat(name.c_str(), &st) != 0) {
    cout << "File " << name << " not found!\n";
    exit(0);
  } else {
    ifstream fin;
    fin.open(name.c_str(), ios::in);
    int i = 0;
    while (!fin.eof()) {
      double tmp;
      fin >> tmp; if ((i%i_step == 0) && (!fin.eof())) x.push_back(tmp);
      fin >> tmp; if ((i%i_step == 0) && (!fin.eof())) y.push_back(tmp);
      i++;
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
  Main routine
----------------------------------------------------------------------------------------------------------------------*/
void work(const string &data_name, const string &pre_name, const int &i_step)
{
  vector<double> x, y, y2;

  // Read data
  read_rare(data_name, x, y, i_step);

  int n = y.size();
  double y1 = (y[1] - y[0])/(x[1] - x[0]);
  double yn = (y[n-1] - y[n-2])/(x[n-1] - x[n-2]);
  y2.resize(n);
  ml_spline(x, y, y1, yn, y2);

  int nn = int((wF - wI)/wS) + 1;
  string file_name = pre_name + data_name;
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
  for (int i = 0; i < data_file_num; ++i)
    work(data_file_name[i], res_pre_name, data_step);
  return 0;
};


//======================================================================================================================
