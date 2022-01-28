#ifndef ASFILESYSTEM_H
#define ASFILESYSTEM_H

#include <string>
#include <vector>
using namespace std;

int GetInpFilesPaths(string folderPath, vector<string> &inpFilesPaths /*out*/);
int CreateNewDirectory(string filePath);
int CopyFileToAnotherDir(string filePath, string newFilePath);

#endif /*ASFILESYSTEM_H*/