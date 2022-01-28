#include <stdlib.h>
#include "asINPtoBDFconverter.h"
#define asCOMMASPERCQUAD4 4
#define asSPACESPERNUMBER 16

INPtoBDFConverter::INPtoBDFConverter(BDFfileData &data, MAT &material, string &plateOffset)
{
  m_data = data;
  m_material = material;
  m_plateOffsetCoeff = plateOffset;
}

void INPtoBDFConverter::convert()
{
  createFormatFileInfo();
  createFormatGrid();
  modifyAllPSHELLsToBDFformat();
  createFormatMaterial();
}

void INPtoBDFConverter::initFormatFileInfo()
{
  m_formatData.fileInfo = &m_data.fileInfo;
}
void INPtoBDFConverter::createFormatFileInfo()
{
  modifyFileInfo();
  initFormatFileInfo();
}
void INPtoBDFConverter::modifyFileInfo()
{
  string BDFcommentSymbol = "$";
  string INPcommentSymbol = "**";

  for (vector<string>::iterator iter = m_data.fileInfo.begin(); iter < m_data.fileInfo.end(); iter++)
  {
    iter->replace(iter->find_first_of(INPcommentSymbol), INPcommentSymbol.length(), BDFcommentSymbol);
  }
}

void ExtractThicknessFromPartName(string &partName, string &thickness)
{
  /*
  Example: HS_SPOS_t7_34mm
  Web_t16_50mm_OP0P21
  */

  reverse(partName.begin(), partName.end());

  int thicknessStartPos = partName.find_first_of("mm");

  if (thicknessStartPos == string::npos)
  {
    thicknessStartPos = partName.find_first_of("MM");
  }
  thicknessStartPos += 2;

  int i = thicknessStartPos;
  while (i < partName.length() && (isdigit(partName.at(i)) || partName.at(i) == '_'))
  {
    thickness += partName.at(i);
    i++;
  }

  reverse(thickness.begin(), thickness.end());

  if (thickness.find_first_of("_") != string::npos)
  {
    thickness.replace(thickness.find_first_of("_"), 1, ".");
  }
}

int formatLinePart(string &line, int requiredSpacesNum, int startIndexPos)
{
  int numberBeginPos = line.find_first_not_of(" ", startIndexPos);
  int numberEndPos = line.find_first_of(" ", numberBeginPos);

  int numberLen = numberEndPos - numberBeginPos;

  int nextNumberBeginPos = line.find_first_not_of(" ", numberEndPos + 1);
  int currentSpacesNum = nextNumberBeginPos - numberEndPos;

  int additionalSpacesNum = requiredSpacesNum - numberLen - currentSpacesNum;

  if (additionalSpacesNum > 0)
  {
    line.insert(numberEndPos + 1, additionalSpacesNum, ' ');
  }
  else
  {
    line.erase(numberEndPos + 1, abs(additionalSpacesNum));
  }
  nextNumberBeginPos += additionalSpacesNum;

  return nextNumberBeginPos;
}

void INPtoBDFConverter::createPSHELLinfo(string &thickness, int PSHELLid, vector<string> &PSHELLinfo /*out*/)
{
  string PSHELLinfoPart1;

  PSHELLinfoPart1 = "PSHELL*  " + to_string(PSHELLid);
  PSHELLinfoPart1.append(16 - to_string(PSHELLid).length(), ' ');
  
  PSHELLinfoPart1 += to_string(m_material.Id);
  PSHELLinfoPart1.append(16 - to_string(m_material.Id).length(), ' ');
  
  PSHELLinfoPart1 += thickness;
  PSHELLinfoPart1.append(16 - thickness.length(), ' ');
  
  PSHELLinfoPart1 += to_string(m_material.Id) + "\n";

  PSHELLinfo.push_back(PSHELLinfoPart1);


  string PSHELLinfoPart2;
  
  double bendingMomentOfInteriaRatioDefaultVal = 1;
  PSHELLinfoPart2 = "*       " + to_string(bendingMomentOfInteriaRatioDefaultVal);

  PSHELLinfoPart2.append(16 - to_string(bendingMomentOfInteriaRatioDefaultVal).length(), ' ');
  PSHELLinfoPart2 += to_string(m_material.Id);
  PSHELLinfoPart2.append(16 - to_string(m_material.Id).length(),  ' ');

  double transverseShearThicknessRatioDefaultVal = 0.833333;
  PSHELLinfoPart2.append(to_string(transverseShearThicknessRatioDefaultVal));

  PSHELLinfoPart2 += "\n";

  PSHELLinfo.push_back(PSHELLinfoPart2);
}

void INPtoBDFConverter::modifyCQUAD4(string &CQUAD4, int PSHELLid, string &CQUAD4secPart /*out*/)
{
  for (int i = 0; i < 4; i++)
  {
    CQUAD4.erase(CQUAD4.find_first_of(","), 1);
  }

  //first part
  int firstNumberBeginPos = CQUAD4.find_first_not_of(" ");
  CQUAD4.erase(0, firstNumberBeginPos);
  CQUAD4.insert(0, "CQUAD4*  ");

  int secNumberBeginPos = formatLinePart(CQUAD4, 16, 8);
  int thirdNumberBeginPos = formatLinePart(CQUAD4, 16, secNumberBeginPos);
  int thirdNumberEndPos = CQUAD4.find_first_of(" ", thirdNumberBeginPos);

  CQUAD4secPart = CQUAD4.substr(thirdNumberEndPos);
  CQUAD4.erase(thirdNumberEndPos);
  CQUAD4secPart.erase(CQUAD4secPart.find_first_of("\n"));

  firstNumberBeginPos = 9;
  int firstNumberEndPos = CQUAD4.find_first_of(" ", 9);
  int firstNumberLen = firstNumberEndPos - firstNumberBeginPos;
  CQUAD4.insert(firstNumberEndPos, asSPACESPERNUMBER - to_string(PSHELLid).length(), ' ');
  CQUAD4.insert(firstNumberEndPos + asSPACESPERNUMBER - firstNumberLen, to_string(PSHELLid));

  CQUAD4 += "\n";

  //second part
  int firstNumBeginPos = CQUAD4secPart.find_first_not_of(" ");
  CQUAD4secPart.erase(0, firstNumBeginPos);
  CQUAD4secPart.insert(0, "*        ");

  int secNumBeginPos = formatLinePart(CQUAD4secPart, 16, 3);

  string theta = "0.";
  CQUAD4secPart += " ";
  CQUAD4secPart += theta;

  int thirdNumBeginPos = formatLinePart(CQUAD4secPart, 16, secNumBeginPos);

  CQUAD4secPart += " ";
  CQUAD4secPart.append(m_currentPlateOffset);
  formatLinePart(CQUAD4secPart, 16, thirdNumBeginPos);

  CQUAD4secPart += "\n";
}

void INPtoBDFConverter::modifyCTRIA3(string &CTRIA3, int PSHELLid, string &CTRIA3secPart/*out*/)
{
  for (int i = 0; i < 3; i++)
  {
    CTRIA3.erase(CTRIA3.find_first_of(","), 1);
  }
  
  //first part
  int firstNumberBeginPos = CTRIA3.find_first_not_of(" ");
  CTRIA3.erase(0, firstNumberBeginPos);
  CTRIA3.insert(0, "CTRIA3*  ");

  int secNumberBeginPos = formatLinePart(CTRIA3, 16, 8);
  int thirdNumberBeginPos = formatLinePart(CTRIA3, 16, secNumberBeginPos);
  int thirdNumberEndPos = CTRIA3.find_first_of(" ", thirdNumberBeginPos);

  CTRIA3secPart = CTRIA3.substr(thirdNumberEndPos);
  CTRIA3.erase(thirdNumberEndPos);
  CTRIA3secPart.erase(CTRIA3secPart.find_first_of("\n"));

  firstNumberBeginPos = 9;
  int firstNumberEndPos = CTRIA3.find_first_of(" ", 9);
  int firstNumberLen = firstNumberEndPos - firstNumberBeginPos;
  CTRIA3.insert(firstNumberEndPos, asSPACESPERNUMBER - to_string(PSHELLid).length(), ' ');
  CTRIA3.insert(firstNumberEndPos + asSPACESPERNUMBER - firstNumberLen, to_string(PSHELLid));

  CTRIA3 += "\n";
  
  //second part
  int firstNumBeginPos = CTRIA3secPart.find_first_not_of(" ");
  CTRIA3secPart.erase(0, firstNumBeginPos);
  CTRIA3secPart.insert(0, "*        ");

  string theta = "0.";
  CTRIA3secPart += " ";
  CTRIA3secPart += theta;

  int secNumBeginPos = formatLinePart(CTRIA3secPart, 16, 3);

  CTRIA3secPart += " ";
  CTRIA3secPart.append(m_currentPlateOffset);
  int thirdNumBeginPos = formatLinePart(CTRIA3secPart, 16, secNumBeginPos);

  CTRIA3secPart += "\n";
}

void INPtoBDFConverter::initPlateOffset(string thickness)
{
  double thicknessNum = atof(thickness.c_str());
  double plateOffsetCoeff = atof(m_plateOffsetCoeff.c_str());
  double plateOffsetNum = thicknessNum * plateOffsetCoeff;
  m_currentPlateOffset = to_string(plateOffsetNum);
}

void INPtoBDFConverter::modifyPSHELLdataToBDFformat(PSHELLdata &data, int PSHELLid, vector<string> &PSHELLinfo)
{
  string thickness;
  ExtractThicknessFromPartName(data.partName, thickness);

  createPSHELLinfo(thickness, PSHELLid, PSHELLinfo);
  initPlateOffset(thickness);

  for (unsigned int i = 0; i < data.CTRIA3vector.size(); i += 2)
  {
    string additionalPart;
    modifyCTRIA3(data.CTRIA3vector[i], PSHELLid, additionalPart);
    data.CTRIA3vector.insert(data.CTRIA3vector.begin() + i + 1, additionalPart);
  }

  for (unsigned int i = 0; i < data.CQUAD4vector.size(); i += 2)
  {
    string additionalPart;
    modifyCQUAD4(data.CQUAD4vector[i], PSHELLid, additionalPart);
    data.CQUAD4vector.insert(data.CQUAD4vector.begin() + i + 1, additionalPart);
  }
}

void INPtoBDFConverter::initFormatPSHELL(PSHELLdata &data, vector<string> &PSHELLinfo, PSHELL &formatPSHELL)
{
  formatPSHELL.PSHELLinfo = PSHELLinfo;
  formatPSHELL.CTRIA3vector = &data.CTRIA3vector;
  formatPSHELL.CQUAD4vector = &data.CQUAD4vector;
}

void INPtoBDFConverter::createFormatPSHELL(PSHELLdata &data, int PSHELLid, PSHELL &formatPSHELL)
{
  vector<string> PSHELLinfo;

  modifyPSHELLdataToBDFformat(data, PSHELLid, PSHELLinfo);
  initFormatPSHELL(data, PSHELLinfo, formatPSHELL);
}

void INPtoBDFConverter::modifyAllPSHELLsToBDFformat()
{
  int PSHELLid = 1;
  for (unsigned int i = 0; i < m_data.PSHELLs.size(); i++)
  {
    PSHELL currentFormatPSHELL;
    createFormatPSHELL(m_data.PSHELLs[i], PSHELLid, currentFormatPSHELL);
    m_formatData.PSEHLLs.push_back(currentFormatPSHELL);
    PSHELLid += 1;
  }
}

void INPtoBDFConverter::modifyGridLine(string &firstGridLinePart, string &secGridLinePart)
{
  for (int i = 0; i < 3; i++)
  {
    firstGridLinePart.erase(firstGridLinePart.find_first_of(","), 1);
  }

  //first part
  int firstNumBeginPos = firstGridLinePart.find_first_not_of(" ");
  firstGridLinePart.erase(0, firstNumBeginPos);
  firstGridLinePart.insert(0, "GRID*    ");

  int secNumberBeginPos = formatLinePart(firstGridLinePart, 31, 6);
  int thirdNumberBeginPos = formatLinePart(firstGridLinePart, 16, secNumberBeginPos);

  int thirdNumberEndPos = firstGridLinePart.find_first_of(" ", thirdNumberBeginPos);
  secGridLinePart = firstGridLinePart.substr(thirdNumberEndPos);
  firstGridLinePart.erase(thirdNumberEndPos);
  firstGridLinePart.append("\n");

  //second part
  int firstNumberBeginPos1 = secGridLinePart.find_first_not_of(" ");
  secGridLinePart.erase(0, firstNumberBeginPos1);
  secGridLinePart.insert(0, "*       ");

  secGridLinePart.erase(secGridLinePart.length() - 1);
  int extraSpacesPos = secGridLinePart.find_last_not_of(" ");
  extraSpacesPos += 1;
  if (extraSpacesPos != secGridLinePart.length())
  {
    secGridLinePart.erase(extraSpacesPos);
  }
  secGridLinePart += "\n";
}

void INPtoBDFConverter::initFormatGrid()
{
  m_formatData.grid = &m_data.grid;
}

void INPtoBDFConverter::modifyGrid()
{
  for (unsigned int i = 0; i < m_data.grid.size(); i += 2){
    string secGridLine;
    modifyGridLine(m_data.grid[i], secGridLine);
    m_data.grid.insert(m_data.grid.begin() + i + 1, secGridLine);
  }
}

void INPtoBDFConverter::createFormatGrid()
{
  modifyGrid();
  initFormatGrid();
}

void INPtoBDFConverter::createFormatMaterial()
{
  string material;
  material.append("MAT1*    ");
  material.append(to_string(m_material.Id));
  material.append(16 - to_string(m_material.Id).length(), ' ');
  material.append(to_string(m_material.E));
  material.append(16 - to_string(m_material.E).length(), ' ');
  double G = m_material.E / (2 * (m_material.NU + 1));
  material.append(to_string(G));
  material.append(16 - to_string(G).length(), ' ');
  material.append(to_string(m_material.NU));
  material.append("\n");

  m_formatData.material = material;
}

BDFfileFormatData &INPtoBDFConverter::getBDFformatData()
{
  return m_formatData;
}