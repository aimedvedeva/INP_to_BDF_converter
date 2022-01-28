#ifndef ASBDFFILE_H
#define ASBDFFILE_H

#include <fstream>
#include "asBDFformatData.h"


class BDFfileCreator{
public:
  BDFfileCreator(BDFfileFormatData &fileData, string filePath);
  void writeBDFfile();

private:
  BDFfileFormatData *m_fileData;
  ofstream m_BDFfile;

private:
  void writeFileInfo();
  void writeAllPSHELLs();
  void writeMAT();
  void writeGrid();
};

#endif /*ASBDFFILE_H*/