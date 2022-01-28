#include <fstream>
#include "asINPfileParser.h"
#define asNEGATIVEVALUE -1
#define asPOSITIVEVALUE 1

INPfileParser::INPfileParser(string filePath)
{
  getFileData(filePath);
}

INPfileParserErrors INPfileParser::testINPFile()
{
  if (getFileDataPos("*NODE", 0) == string::npos)
  {
    return INPfileParserErrors_INCLUDE_OR_INSTANCE_FILE;
  }
  if (getFileDataPos("SC6R", 0) != string::npos || getFileDataPos("C3D6", 0) != string::npos || getFileDataPos("SC8R", 0) != string::npos)
  {
    return INPfileParserErrors_FILE_WITH_WRONG_ELEMENTS;
  }
  return INPfileParserErrors_RIGHT_FILE;
}

void INPfileParser::parseFile()
{
  INPfileParserErrors error = testINPFile();
  SetINPfileParsingError(error);

  if (isErrorExisted())
  {
    return;
  }

  readFileInfo();
  readGrid();

  if (m_CTRIA3pos > 0)
  {
    m_CQUAD4pos = getFileDataPos("S4R", m_CTRIA3pos);
  }
  else
  {
    m_CTRIA3pos = asNEGATIVEVALUE;
  }
  
  readAllPSHELLs();
}

void INPfileParser::getFileData(string filePath)
{
  ifstream file(filePath);

  string currentString;
  while (getline(file, currentString)){
    currentString += '\n';
    m_fileData.push_back(currentString);
    currentString.clear();
  }
}

void INPfileParser::readFileInfo()
{
  int i;
  for (i = 0; m_fileData[i + 1].find("*NODE") == string::npos; i++)
  {
    m_fileInfo.push_back(m_fileData[i]);
  }
  m_fileData.begin() = m_fileData.erase(m_fileData.begin(), m_fileData.begin() + i + 1);
}

void INPfileParser::readGrid()
{
  int gridStartIndex = getFileDataPos("*NODE", 0);

  int i;

  for (i = gridStartIndex + 1; m_fileData[i].find("*ELEMENT") == string::npos; i++)
  {
    if (m_fileData[i].find("**") == string::npos)
    {
      m_grid.push_back(m_fileData[i]);
    }
  }

  if (m_fileData[i].find("S3R") != string::npos)
  {
    m_CTRIA3pos = i;
  }
  else
  {
    m_CQUAD4pos = i;
  }
}

void INPfileParser::readPSHELLdata(PSHELLdata &data, bool isCTRIA3table)
{
  if (isCTRIA3table)
  {
    int i;
    for (i = m_CTRIA3pos + 1; m_fileData[i].find_first_of("*ELEMENT") == string::npos; i++)
    {
      data.CTRIA3vector.push_back(m_fileData[i]);
    }
    m_CTRIA3pos = i;
  }
  else
  {
    int i;
    for (i = m_CQUAD4pos + 1; m_fileData[i].find_first_of("*ELEMENT") == string::npos; i++)
    {
      data.CQUAD4vector.push_back(m_fileData[i]);
    }
    m_CQUAD4pos = i;
  }
}

void INPfileParser::readAllPSHELLs()
{
  string currentPartName;
  int startPos = m_CTRIA3pos > 0 ? m_CTRIA3pos : m_CQUAD4pos;
  
  bool isFirstIteration = true;
  
  while (m_CTRIA3pos >= 0 && m_fileData[m_CTRIA3pos].find("S4R") == string::npos)
  {
    if (isFirstIteration)
    {
      getPartName(m_fileData[startPos], currentPartName);
      isFirstIteration = false;
    }
    else
    {
      int oldCTRIA3pos = m_CTRIA3pos;
      passComments(m_CTRIA3pos);
      int shiftPos = abs(oldCTRIA3pos - m_CTRIA3pos);
      m_CQUAD4pos += shiftPos;

      getPartName(m_fileData[m_CTRIA3pos], currentPartName);
    }


    if (currentPartName.find("*END") != string::npos){
      return;
    }

    //read CTRIA3 table
    PSHELLdata currentTable;
    currentTable.partName = currentPartName;


    int oldCTRIA3pos = m_CTRIA3pos;
    readPSHELLdata(currentTable, true);
    m_fileData.erase(m_fileData.begin() + oldCTRIA3pos, m_fileData.begin() + m_CTRIA3pos);
    int shiftPos = abs(oldCTRIA3pos - m_CTRIA3pos);
    m_CTRIA3pos -= shiftPos;
    m_CQUAD4pos -= shiftPos;

    //read CQUAD4 table
    int currentCQIAD4pos = getFileDataPos(currentPartName, m_CQUAD4pos);
    if (currentCQIAD4pos != asNEGATIVEVALUE)
    {
      int oldCQUAD4pos = m_CQUAD4pos;
      readPSHELLdata(currentTable, false);
      m_fileData.erase(m_fileData.begin() + oldCQUAD4pos, m_fileData.begin() + m_CQUAD4pos);
      int shiftPos = abs(oldCQUAD4pos - m_CQUAD4pos);
      m_CQUAD4pos -= shiftPos;
    }

    m_tables.push_back(currentTable);
  } 

  getPartName(m_fileData[startPos], currentPartName);
  while (m_fileData[m_CQUAD4pos].find("*END") == string::npos)
  {
    //read CQIAD4 table
    PSHELLdata currentTable;
    currentTable.partName = currentPartName;
    
    int oldCQUAD4pos = m_CQUAD4pos;
    readPSHELLdata(currentTable, false);
    m_fileData.erase(m_fileData.begin() + oldCQUAD4pos, m_fileData.begin() + m_CQUAD4pos);
    int shiftPos = abs(oldCQUAD4pos - m_CQUAD4pos);
    m_CQUAD4pos -= shiftPos;

    m_tables.push_back(currentTable);

    passComments(m_CQUAD4pos);
    getPartName(m_fileData[m_CQUAD4pos], currentPartName);
  }
}

void INPfileParser::passComments(int &currentDataPos)
{
  while (m_fileData[currentDataPos].find("**") != string::npos || m_fileData[currentDataPos].find("***") != string::npos)
  {
    currentDataPos += 1;
  }
}

void INPfileParser::getPartName(string str, string &partName)
{
  size_t partNamePos = str.find_last_of("=");
  
  partName = str.substr(partNamePos + 1, str.size() - partNamePos);

  partName.erase(partName.length() - 1);
}

int INPfileParser::getFileDataPos(string substring, int startIndex)
{
  for (int i = startIndex; i < (int)m_fileData.size(); i++)
  {
    if (m_fileData[i].find(substring) != string::npos){
      return i;
    }
  }
  return asNEGATIVEVALUE;
}

void INPfileParser::initBDFfileData(BDFfileData &data)
{
  data.fileInfo = m_fileInfo;
  data.grid = m_grid;
  data.PSHELLs = m_tables;
}