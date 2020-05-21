#pragma once
#include <string>
#include <filesystem>

int GetSectorSize(std::wstring const &drive);
bool ReadSector(char *&buffer, std::wstring const &drive, int const &numSec,
                int const &secSize);
void Flush(std::istream& fi);
void ConvertShortName(std::filesystem::path const &fPath, char buffer[11]);