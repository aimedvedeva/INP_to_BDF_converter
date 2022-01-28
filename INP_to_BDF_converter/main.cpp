#include "asFileSystem.h"
#include "asINPfileParser.h"
#include "asINPtoBDFconverter.h"
#include "asBDFfileCreator.h"

void PreprocessStaffData(MAT &material, double &plateOffset, string &currentDirectoryName)
{
  string staffFileName = "converterInfo.txt";
  ifstream staffFile(staffFileName);

  getline(staffFile, currentDirectoryName);
  staffFile >> material.Id >> material.E >> material.NU >> plateOffset;
}
 
void GetNewFolderPath(string currentDirectoryPath, string folderName, string &BDFfolderPath /*out*/)
{
  BDFfolderPath = currentDirectoryPath + "\\" + folderName;
}

void GetNewfileName(vector<string>::iterator &currentINPfile, string BDFfolderPath, string extension, string &BDFfileName /*out*/)
{
  BDFfileName = BDFfolderPath + "\\" + currentINPfile->substr(currentINPfile->find_last_of("\\") + 1);
  BDFfileName.replace(BDFfileName.length() - 4, 4, "." + extension);
}


int main(void)
{
    MAT material;
    double plateOffset;
    string currentDirectoryPath;

    PreprocessStaffData(material, plateOffset, currentDirectoryPath);

    vector<string> filePaths;
    GetInpFilesPaths(currentDirectoryPath, filePaths);

    string BDFfolderPath;
    GetNewFolderPath(currentDirectoryPath, "BDF files", BDFfolderPath);


    bool isBDFfileCreated = true;
    bool isSpecificFileCreated = false;

    for (vector<string>::iterator iter = filePaths.begin(); iter < filePaths.end(); iter++)
    {
      INPfileParser parser(*iter);
      parser.parseFile();

      INPfileParserErrors error = GetLastParserError();
      if (error == INPfileParserErrors_INCLUDE_OR_INSTANCE_FILE){
        continue;
      }
      else if (error == INPfileParserErrors_FILE_WITH_WRONG_ELEMENTS)
      {
        string additionalFolderPath;
        GetNewFolderPath(currentDirectoryPath, "Specific files", additionalFolderPath);

        string copiedFileName;
        GetNewfileName(iter, additionalFolderPath, "inp", copiedFileName);

        if (!isSpecificFileCreated)
        {
          CreateNewDirectory(additionalFolderPath);
          isSpecificFileCreated = true;
        }

        CopyFileToAnotherDir(*iter, copiedFileName);
        continue;
      }

      if (isBDFfileCreated)
      {
        CreateNewDirectory(BDFfolderPath);
        isBDFfileCreated = true;
      }

      BDFfileData data;
      parser.initBDFfileData(data);

      string plateOffsetStr = to_string(plateOffset);
      INPtoBDFConverter converter(data, material, plateOffsetStr);
      converter.convert();

      BDFfileFormatData formatData;
      formatData = converter.getBDFformatData();

      string BDFfileName;
      GetNewfileName(iter, BDFfolderPath, "bdf", BDFfileName);

      BDFfileCreator BDFfile(formatData, BDFfileName);
      BDFfile.writeBDFfile();
    }
  
  return 0;
}