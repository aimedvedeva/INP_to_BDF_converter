#include <Windows.h>
#include <iostream>
#include <string.h>
#include "asFileSystem.h"

int GetInpFilesPaths(string folderPath, vector<string> &inpFilesPaths /*out*/)
{
  WIN32_FIND_DATAA findFileData;
  HANDLE           hFind;
  string           findFileName("*.inp");

  folderPath += "\\";

  hFind = FindFirstFileA((folderPath + findFileName).c_str(), &findFileData);

  if (hFind == INVALID_HANDLE_VALUE)
  {
    cout << "Invalid file handle" << GetLastError() << endl;
    return 0;
  }
  else
  {
    inpFilesPaths.push_back(folderPath + findFileData.cFileName);

    BOOL returnValue = FindNextFileA(hFind, &findFileData);
    while (returnValue == 1)
    {
      inpFilesPaths.push_back(folderPath + findFileData.cFileName);
      returnValue = FindNextFileA(hFind, &findFileData);
    }
    if (GetLastError() == ERROR_NO_MORE_FILES)
    {
      FindClose(hFind);
      return 1;
    }
    else
    {
      cout << "Invalid file handle" << GetLastError() << endl;
      return 0;
    }
  }
}

int CreateNewDirectory(string filePath)
{
  int result = CreateDirectoryA(filePath.c_str(), NULL);

  if (GetLastError() == ERROR_ALREADY_EXISTS)
  {
    return -1;
  }
  else if (GetLastError() == ERROR_PATH_NOT_FOUND)
  {
    return -1;
  }
  else if (result == false){
    return -1;
  }

  return 1;
}

int CopyFileToAnotherDir(string filePath, string newFilePath)
{
  int result = CopyFileA(filePath.c_str(), newFilePath.c_str(), FALSE);
  if (GetLastError() == ERROR_ALREADY_EXISTS)
  {
    return -1;
  }
  else if (GetLastError() == ERROR_PATH_NOT_FOUND)
  {
    return -1;
  }
  else if (result == false){
    return -1;
  }
}

