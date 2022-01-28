#ifndef ASINPFORMATDATA_H
#define ASINPFORMATDATA_H

#include <string>
#include <vector>
using namespace std;

struct PSHELLdata
{
  string partName;
  vector<string> CTRIA3vector;
  vector<string> CQUAD4vector;
};

struct BDFfileData
{
  vector<string> fileInfo;
  vector<string> grid;
  vector<PSHELLdata> PSHELLs;
};

#endif /*ASINPFORMATDATA_H*/ 