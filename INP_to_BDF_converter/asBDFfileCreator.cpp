#include <fstream>
#include <Windows.h>
#include "asBDFfileCreator.h"

BDFfileCreator::BDFfileCreator(BDFfileFormatData &fileData, string filePath)
{
  m_fileData = &fileData;
  m_BDFfile.open(filePath);
}

void BDFfileCreator::writeBDFfile()
{
  writeFileInfo();
  writeAllPSHELLs();
  writeMAT();
  writeGrid();
}

void BDFfileCreator::writeFileInfo()
{
  copy(m_fileData->fileInfo->begin(), m_fileData->fileInfo->end(), ostream_iterator<string>(m_BDFfile));
}

void BDFfileCreator::writeAllPSHELLs()
{
  for (vector<PSHELL>::iterator PSHELLiter = m_fileData->PSEHLLs.begin(); PSHELLiter < m_fileData->PSEHLLs.end(); PSHELLiter++)
  {
    
    copy(PSHELLiter->PSHELLinfo.begin(), PSHELLiter->PSHELLinfo.end(), ostream_iterator<string>(m_BDFfile));
    copy(PSHELLiter->CTRIA3vector->begin(), PSHELLiter->CTRIA3vector->end(), ostream_iterator<string>(m_BDFfile));
    copy(PSHELLiter->CQUAD4vector->begin(), PSHELLiter->CQUAD4vector->end(), ostream_iterator<string>(m_BDFfile));
  }
}

void BDFfileCreator::writeMAT()
{
  m_BDFfile << m_fileData->material;
}

void BDFfileCreator::writeGrid()
{
  copy(m_fileData->grid->begin(), m_fileData->grid->end(), ostream_iterator<string>(m_BDFfile));
}





