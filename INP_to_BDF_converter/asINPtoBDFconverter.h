#ifndef ASINPTOBDFCONVERTER_H
#define ASINPTOBDFCONVERTER_H

#include "asINPformatData.h"
#include "asBDFformatData.h"

class INPtoBDFConverter
{
public:
  INPtoBDFConverter(BDFfileData &data, MAT &material, string &plateOffset);

  void convert();
  BDFfileFormatData &getBDFformatData();

private:
  BDFfileData m_data;
  BDFfileFormatData m_formatData;
  MAT m_material;
  string m_plateOffsetCoeff;
  string m_currentPlateOffset;
  

private:
  void modifyFileInfo();
  void initFormatFileInfo();
  void createFormatFileInfo();

  void initPlateOffset(string thickness);
  void createPSHELLinfo(string &thickness, int PSHELLid, vector<string> &PSHELLinfo /*out*/);
  void modifyCTRIA3(string &CTRIA3, int PSHELLid, string &CTRIA3secPart);
  void modifyCQUAD4(string &CQUAD4, int PSHELLid, string &CQUAD4secPart);
  void initFormatPSHELL(PSHELLdata &data, vector<string> &PSHELLinfo, PSHELL &formatPSHELL);
  void modifyPSHELLdataToBDFformat(PSHELLdata &data, int PSHELLid, vector<string> &PSHELLinfo);
  void createFormatPSHELL(PSHELLdata &data, int PSHELLid, PSHELL &formatPSHELL);
  void modifyAllPSHELLsToBDFformat();

  void modifyGridLine(string &firstGridLinePart, string &secGridLinePart);
  void modifyGrid();
  void initFormatGrid();
  void createFormatGrid();

  void createFormatMaterial();
};

#endif /*ASINPTOBDFCONVERTER_H*/