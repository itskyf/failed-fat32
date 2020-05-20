#pragma once
#include <string>

int GetSectorSize(std::wstring const &drive);
bool ReadSector(char *&buffer, std::wstring const &drive, int const &numSec,
                int const &secSize);
void Flush(std::istream& fi);