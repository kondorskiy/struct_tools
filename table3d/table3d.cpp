
/*====================================================================

  DEFINITION OF THE TABULATED FUNCTION OF TWO ARGUMENTS OBJECT:

  The object loads and keeps the array of 3D real function.

  ACKNOWLEDGEMENT(S): Alexey D. Kondorskiy,
    P.N.Lebedev Physical Institute of the Russian Academy of Science.
    E-mail: kondorskiy@lebedev.ru, kondorskiy@gmail.com.

  Last modified: November 11, 2020.

====================================================================*/

//***** For testing. *************************************************
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <complex>
#include <string>
#include <math.h>
#include <sys/stat.h>
#include <vector>
#include <sstream>
using namespace std;
//*******************************************************************/


class Table3D
{
  private: vector<double> a_x;          // Array of 1st argument.
  private: vector<double> a_y;          // Array of 2nd argument.
  private: vector<vector<double> > a_z; // Array of function values.
  private: bool transpose;              // Flag to transpose result.


  /*------------------------------------------------------------------
    Constructor & Destructor.
  ------------------------------------------------------------------*/
  public: Table3D()
    { clear(); }

  public: ~Table3D()
    { clear(); }


  /*------------------------------------------------------------------
    Clear object.
  ------------------------------------------------------------------*/
  public: void clear()
  {
    a_x.clear(); a_y.clear();
    for(int i = 0; i < a_z.size(); ++i)
      a_z[i].clear();
    a_z.clear();
    transpose = false;
  }


  /*------------------------------------------------------------------
    Initialization by loading the data from file.
  ------------------------------------------------------------------*/
  public: void init(
    string file_name)   // Name of file to load data.
  {
    clear();

    struct stat st;
    if(stat(file_name.c_str(), &st) != 0) {
      cout << "File " << file_name << " not found!\n";
      exit(0);
    } else {
      ifstream fin(file_name.c_str());
      string s;

      bool first_read = true;
      vector<double> z0;
      z0.clear();

      int ix = 0;
      int iy = 0;
      int ifast = 0;

      while (getline(fin, s))
        if ( !s.empty() ) {     // Work with not empty string.

          istringstream ss;
          ss.str(s);

          if (first_read) {               // First read.

            double tmp;
            ss >> tmp; a_x.push_back(tmp);
            ss >> tmp; a_y.push_back(tmp);
            ss >> tmp; z0.push_back(tmp);
            first_read = false;

          } else {                        // Ordinary read.

            double xtmp; ss >> xtmp;
            double ytmp; ss >> ytmp;
            double ztmp; ss >> ztmp;

            if (a_x[ix] == xtmp) {        // 2nd variable is fast.

              ++iy;
              ifast = 2;

              if ( (a_y.size() - 1) < iy ) {
                a_y.push_back(ytmp);
              } else if (a_y[iy] != ytmp) {
                cout << "Y grid in file " << file_name
                  << " is corrupted!\n";
                fin.close();
                exit(0);
              }

              z0.push_back(ztmp);

            } else if (a_y[iy] == ytmp) { // 1st variable is fast.

              ++ix;
              ifast = 1;

              if ( (a_x.size() - 1) < ix ) {
                a_x.push_back(xtmp);
              } else if (a_x[ix] != xtmp) {
                cout << "X grid in file " << file_name
                  << " is corrupted!\n";
                fin.close();
                exit(0);
              }

              z0.push_back(ztmp);

            } else if ( (a_x[ix] != xtmp) && (a_y[iy] != ytmp) )

              if(a_y[0] == ytmp) {        // Change 1st slow variable.

                a_z.push_back(z0);
                z0.clear();
                a_x.push_back(xtmp);
                ++ix;
                iy = 0;
                z0.push_back(ztmp);

              } else if (a_x[0] == xtmp) { // Change 2nd slow variable.
                a_z.push_back(z0);
                z0.clear();
                a_y.push_back(ytmp);
                ++iy;
                ix = 0;
                z0.push_back(ztmp);
              } else {
                cout << "Irregular grid in file " << file_name
                  << "!\n";
                fin.close();
                exit(0);
              }
          }
        }                       // Work with not empty string.

      // Add last column of z-data.
      a_z.push_back(z0);
      z0.clear();

      fin.close();

      // Check the fast and slow arguments
      // and set the index order in "a_z" array.
      if (ifast == 1)
        transpose = true;

      if (ifast == 2)
        transpose = false;

      if (ifast == 0) {
        cout << "No order in file " << file_name << "!\n";
        exit(0);
      }

      // Check square form.
      for (int i = 1; i < a_z.size(); ++i)
        if (a_z[i].size() != a_z[0].size()) {
          cout << "Not square matrix in " << file_name << "!\n";
          cout << "Size of " << i << " is not equal to size of 0.";
          exit(0);
        }

      /* Test output.
      cout << "ifast = " << ifast 
	    << "  transpose = " << transpose << "\n";
      cout << "Z size = " << a_z.size() << "\n";
      for(int i = 0; i < a_z.size(); ++i) cout << "  Z["
        << i << "] size = " << a_z[i].size() << "\n"; // */

    }
  }


  /*------------------------------------------------------------------
    Array sizes.
  ------------------------------------------------------------------*/
  public: int getXNum() { return a_x.size(); }
  public: int getYNum() { return a_y.size(); }

  public: void getZnum(int &nx, int &ny) {
    if(transpose)
      { ny = a_z.size(); nx = a_z[0].size(); }
    else
      { nx = a_z.size(); ny = a_z[0].size(); }

    return;
  }


  /*------------------------------------------------------------------
    Get array values.
  ------------------------------------------------------------------*/
  public: double getX(int i) { return a_x[i]; }
  public: double getY(int j) { return a_y[j]; }

  public: double getZ(int i, int j)
  {
    if(transpose)
      return a_z[j][i];
    else
      return a_z[i][j];
  }


}; //=================================================================



/*********************************************************************
  Test program.
*********************************************************************/
int main(int argc, char **argv)
{
  int nx, ny;

  // --- Original. ---------------------------------------------------
  Table3D if2;
  if2.init("data.dat");

  cout << "\nSizes:\nx: " << if2.getXNum()
    << "; y : " << if2.getYNum() << "\n";

  if2.getZnum(nx, ny);
  cout << "Sizes of matrix: " << nx << " x " << ny << "\n";

  cout << "\n";
  for(int i = 0; i < nx; ++i)
    cout << "  x[" << i << "] = " << if2.getX(i) << "\n";
    
  cout << "\n";
  for(int i = 0; i < ny; ++i)
    cout << "  y[" << i << "] = " << if2.getY(i) << "\n";

  cout << "\nTable:\n";
  for(int i = 0; i < nx; ++i)
  {
    for(int j = 0; j < ny; ++j) cout << if2.getX(i) << " "
      << if2.getY(j) << " " << if2.getZ(i, j) << "\n";
    cout << "\n";
  }


  // --- Transposed. -------------------------------------------------
  cout << "\n---------------------------------------------------\n\n";

  Table3D t3d;
  t3d.init("dataT.dat");

  cout << "\nSizes:\nx: " << t3d.getXNum()
    << "; y : " << t3d.getYNum() << "\n";

  t3d.getZnum(nx, ny);
  cout << "Sizes of matrix: " << nx << " x " << ny << "\n";

  cout << "\n";
  for(int i = 0; i < nx; ++i)
    cout << "  x[" << i << "] = " << t3d.getX(i) << "\n";

  cout << "\n";
  for(int i = 0; i < ny; ++i)
    cout << "  y[" << i << "] = " << t3d.getY(i) << "\n";

  cout << "\nTable:\n";
  for(int i = 0; i < nx; ++i) {
    for(int j = 0; j < ny; ++j) cout << t3d.getX(i) << " "
      << t3d.getY(j) << " " << t3d.getZ(i, j) << "\n";
    cout << "\n";
  }

  return 0;
}   // */


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Test file "data.dat":

1 1 101
1 2 102
1 3 103
1 4 104

2 1 201
2 2 202
2 3 203
2 4 204

3 1 301
3 2 302
3 3 303
3 4 304


Test file "dataT.dat":

1 1 101
2 1 102
3 1 103
4 1 104

1 2 201
2 2 202
3 2 203
4 2 204

1 3 301
2 3 302
3 3 303
4 3 304

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//==================================================================*/
