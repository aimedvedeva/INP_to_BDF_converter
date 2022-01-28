#ifndef ASBDFFILEPARSER_H
#define ASBDFFILEPARSER_H

#include "asINPformatData.h"
#include "asINPfileParserErrors.h"

class INPfileParser
{
public:
  INPfileParser(string filePath);
  void parseFile();
  void initBDFfileData(BDFfileData &data);

private:
  vector<string> m_fileData;
  vector<string> m_fileInfo;
  vector<string> m_grid;
  vector<PSHELLdata> m_tables;
  
  int m_CTRIA3pos = 0;
  int m_CQUAD4pos = 0;

private:
  INPfileParserErrors testINPFile();
  void getFileData(string filePath);

  void readFileInfo();
  void readGrid();
  void readAllPSHELLs();

  void readPSHELLdata(PSHELLdata &data, bool isCTRIA3table);
  void getPartName(string str, string &newPartName);
  void passComments(int &currentDataPos);
  int getFileDataPos(string substring, int startIndex);
};

#endif /*ASBDFFILEPARSER_H*/