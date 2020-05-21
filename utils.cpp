#include "utils.h"

#include <fstream>
#include <iostream>

#include "Windows.h"

void Flush(std::istream &fi) {
  int ch;
  while ((ch = fi.get()) != '\n' && ch != EOF)
    ;
}

int GetSectorSize(std::wstring const &drive) {
  HANDLE hDevice = CreateFile((L"\\\\.\\" + drive).c_str(), 0, 0, nullptr,
                              OPEN_EXISTING, 0, nullptr);
  DWORD outsize;
  STORAGE_PROPERTY_QUERY storageQuery;
  memset(&storageQuery, 0, sizeof(STORAGE_PROPERTY_QUERY));
  storageQuery.PropertyId = StorageAccessAlignmentProperty;
  storageQuery.QueryType = PropertyStandardQuery;

  STORAGE_ACCESS_ALIGNMENT_DESCRIPTOR diskAlignment = {0};
  memset(&diskAlignment, 0, sizeof(STORAGE_ACCESS_ALIGNMENT_DESCRIPTOR));
  if (!DeviceIoControl(hDevice, IOCTL_STORAGE_QUERY_PROPERTY, &storageQuery,
                       sizeof(STORAGE_PROPERTY_QUERY), &diskAlignment,
                       sizeof(STORAGE_ACCESS_ALIGNMENT_DESCRIPTOR), &outsize,
                       nullptr)) {
    std::cerr << "Error" << std::endl;
    exit(-1);
  }
  return diskAlignment.BytesPerLogicalSector;
}

bool ReadSector(char *&buffer, std::wstring const &drive, int const &numSec,
                int const &secSize) {
  DWORD bytesRead;
  HANDLE device = CreateFile((L"\\\\.\\" + drive).c_str(), GENERIC_READ,
                             FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
                             OPEN_EXISTING, 0, nullptr);
  if (device == INVALID_HANDLE_VALUE) {
    std::cout << "Read error, code: " << GetLastError() << std::endl;
    return false;
  }
  SetFilePointer(device, 0, nullptr, FILE_BEGIN);
  return ReadFile(device, buffer, secSize * numSec, &bytesRead, NULL);
}

void WriteBuffer(char const *buffer, int const &length) {
  std::ofstream fo("output.bin", std::ios::binary);
  fo.write(buffer, length);
  fo.close();
}

void ConvertShortName(std::filesystem::path const &fPath, char buffer[11]) {
  std::string fName = fPath.stem().u8string();
  std::string ext = fPath.extension().u8string();

  memset(buffer, ' ', sizeof(buffer));
  if (!ext.empty()) {
    for (int i = 0; i < 3; ++i) {
      buffer[8 + i] = toupper(ext[i]);
    }
  }

  // Write the filename
  if (fName.length() > 8) {
    // Write the weird tilde thing.
    for (int i = 0; i < 6; i++) {
      buffer[i] = toupper(fName[i]);
    }
    buffer[6] = '~';
    buffer[7] = '1';  // TODO need to enumerate files and increment.
  } else {
    for (int i = 0; i < fName.length(); ++i) {
      buffer[i] = toupper(fName[i]);
    }
  }
}

void CommandParser() {
  std::string cmd = "", path = "";
  std::getline(std::cin, cmd, ' ');
  std::getline(std::cin, path);
  // TODO trim
}