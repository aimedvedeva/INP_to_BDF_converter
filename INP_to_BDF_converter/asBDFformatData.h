#ifndef ASBDFFORMATDATA_H
#define ASBDFFORMATDATA_H

#include <string>
#include <vector>
using namespace std;

struct PSHELL
{
  vector<string> PSHELLinfo;
  vector<string> *CTRIA3vector;
  vector<string> *CQUAD4vector;
};

struct MAT
{
  int Id;
  double E;
  double NU;
};

struct BDFfileFormatData
{
  vector<string> *fileInfo;
  vector<string> *grid;
  string material;
  vector<PSHELL> PSEHLLs;
};

#endif /*ASBDFFORMATDATA_H*/ 