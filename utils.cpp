#include "utils.h"

#include <Windows.h>

#include <iostream>

void Flush(std::istream& fi) {
  int ch;
  while ((ch = fi.get()) != '\n' && ch != EOF)
    ;
}

//int GetSectorSize(std::wstring const& drive) {
//  HANDLE hDevice = CreateFile((L"\\\\.\\" + drive).c_str(), 0, 0, nullptr,
//                              OPEN_EXISTING, 0, nullptr);
//  DWORD outsize;
//  STORAGE_PROPERTY_QUERY storageQuery;
//  memset(&storageQuery, 0, sizeof(STORAGE_PROPERTY_QUERY));
//  storageQuery.PropertyId = StorageAccessAlignmentProperty;
//  storageQuery.QueryType = PropertyStandardQuery;
//
//  STORAGE_ACCESS_ALIGNMENT_DESCRIPTOR diskAlignment = {0};
//  memset(&diskAlignment, 0, sizeof(STORAGE_ACCESS_ALIGNMENT_DESCRIPTOR));
//  if (!DeviceIoControl(hDevice, IOCTL_STORAGE_QUERY_PROPERTY, &storageQuery,
//                       sizeof(STORAGE_PROPERTY_QUERY), &diskAlignment,
//                       sizeof(STORAGE_ACCESS_ALIGNMENT_DESCRIPTOR), &outsize,
//                       nullptr)) {
//    std::cerr << "Error" << std::endl;
//    return 0;
//  }
//  return diskAlignment.BytesPerLogicalSector;
//}