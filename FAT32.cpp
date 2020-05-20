#include "FAT32.h"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "utils.h"

uint32_t DivRoundup(uint32_t size, uint32_t n) {
  // Overflow-resistant
  uint32_t res = size / n;
  if (res * n != size) ++res;
  return res;
}

uint16_t CalSc(size_t sizeMb) {
  static const int VolSize[] = {33, 65, 129, 257, 8 << 10, 16 << 10, 32 << 10};
  static const uint16_t ClusSize[] = {0,       512,     1 << 10, 2 << 10,
                                      4 << 10, 8 << 10, 16 << 10};
  for (int i = 0; i < 6; ++i) {
    if (sizeMb < VolSize[i]) return ClusSize[i];
  }
  return 0;
}

void BootSector::Write(std::fstream &file) {
  file.write(reinterpret_cast<char *>(&BS_Jump_Code), sizeof(BS_Jump_Code));
  file.write(reinterpret_cast<char *>(&BS_OEM_ID), sizeof(BS_OEM_ID));
  file.write(reinterpret_cast<char *>(&BytesPerSector), sizeof(BytesPerSector));
  file.write(reinterpret_cast<char *>(&Sc), sizeof(Sc));
  file.write(reinterpret_cast<char *>(&Sb), sizeof(Sb));
  file.write(reinterpret_cast<char *>(&Nf), sizeof(Nf));
  file.write(reinterpret_cast<char *>(&RootEntryCount), sizeof(RootEntryCount));
  file.write(reinterpret_cast<char *>(&TotalSectors16), sizeof(TotalSectors16));
  file.write(reinterpret_cast<char *>(&Media), sizeof(Media));
  file.write(reinterpret_cast<char *>(&FATSize16), sizeof(FATSize16));
  file.write(reinterpret_cast<char *>(&SectorsPerTrack),
             sizeof(SectorsPerTrack));
  file.write(reinterpret_cast<char *>(&NumberOfHeads), sizeof(NumberOfHeads));
  file.write(reinterpret_cast<char *>(&HiddenSectors), sizeof(HiddenSectors));
  file.write(reinterpret_cast<char *>(&Sv), sizeof(Sv));
  file.write(reinterpret_cast<char *>(&Sf), sizeof(Sf));
  file.write(reinterpret_cast<char *>(&ExtFlags), sizeof(ExtFlags));
  file.write(reinterpret_cast<char *>(&FSVersion), sizeof(FSVersion));
  file.write(reinterpret_cast<char *>(&RootCluster), sizeof(RootCluster));
  file.write(reinterpret_cast<char *>(&FSInfile), sizeof(FSInfile));
  file.write(reinterpret_cast<char *>(&BkBootSec), sizeof(BkBootSec));
  file.write(reinterpret_cast<char *>(&Reserved), sizeof(Reserved));
  file.write(reinterpret_cast<char *>(&BS_DriveNumber), sizeof(BS_DriveNumber));
  file.write(reinterpret_cast<char *>(&BS_Reserved1), sizeof(BS_Reserved1));
  file.write(reinterpret_cast<char *>(&BS_BootSig), sizeof(BS_BootSig));
  file.write(reinterpret_cast<char *>(&BS_VolumeID), sizeof(BS_VolumeID));
  file.write(reinterpret_cast<char *>(&BS_VolumeLabel), sizeof(BS_VolumeLabel));
  file.write(reinterpret_cast<char *>(&BS_FileSystemType),
             sizeof(BS_FileSystemType));
}

void BootSector::Read(std::fstream &file) {
  file.read(reinterpret_cast<char *>(&BS_Jump_Code), sizeof(BS_Jump_Code));
  file.read(reinterpret_cast<char *>(&BS_OEM_ID), sizeof(BS_OEM_ID));
  file.read(reinterpret_cast<char *>(&BytesPerSector), sizeof(BytesPerSector));
  file.read(reinterpret_cast<char *>(&Sc), sizeof(Sc));
  file.read(reinterpret_cast<char *>(&Sb), sizeof(Sb));
  file.read(reinterpret_cast<char *>(&Nf), sizeof(Nf));
  file.read(reinterpret_cast<char *>(&RootEntryCount), sizeof(RootEntryCount));
  file.read(reinterpret_cast<char *>(&TotalSectors16), sizeof(TotalSectors16));
  file.read(reinterpret_cast<char *>(&Media), sizeof(Media));
  file.read(reinterpret_cast<char *>(&FATSize16), sizeof(FATSize16));
  file.read(reinterpret_cast<char *>(&SectorsPerTrack),
            sizeof(SectorsPerTrack));
  file.read(reinterpret_cast<char *>(&NumberOfHeads), sizeof(NumberOfHeads));
  file.read(reinterpret_cast<char *>(&HiddenSectors), sizeof(HiddenSectors));
  file.read(reinterpret_cast<char *>(&Sv), sizeof(Sv));
  file.read(reinterpret_cast<char *>(&Sf), sizeof(Sf));
  file.read(reinterpret_cast<char *>(&ExtFlags), sizeof(ExtFlags));
  file.read(reinterpret_cast<char *>(&FSVersion), sizeof(FSVersion));
  file.read(reinterpret_cast<char *>(&RootCluster), sizeof(RootCluster));
  file.read(reinterpret_cast<char *>(&FSInfo), sizeof(FSInfo));
  file.read(reinterpret_cast<char *>(&BkBootSec), sizeof(BkBootSec));
  file.read(reinterpret_cast<char *>(&Reserved), sizeof(Reserved));
  file.read(reinterpret_cast<char *>(&BS_DriveNumber), sizeof(BS_DriveNumber));
  file.read(reinterpret_cast<char *>(&BS_Reserved1), sizeof(BS_Reserved1));
  file.read(reinterpret_cast<char *>(&BS_BootSig), sizeof(BS_BootSig));
  file.read(reinterpret_cast<char *>(&BS_VolumeID), sizeof(BS_VolumeID));
  file.read(reinterpret_cast<char *>(&BS_VolumeLabel), sizeof(BS_VolumeLabel));
  file.read(reinterpret_cast<char *>(&BS_FileSystemType),
            sizeof(BS_FileSystemType));
}

FAT32::FAT32(size_t sizeMb, std::string const &imgPath)
    : file(imgPath, std::ios::out | std::ios::binary),
      bs(new BootSector),
      rdet(nullptr),
      afat(nullptr) {
  memset(bs->BS_Jump_Code, 0, 3);
  memset(bs->BS_OEM_ID, 0, 8);

  bs->BytesPerSector = uint16_t(512);
  bs->Sc = 8;  // CalSc(sizeMb);
  assert(sizeMb > 33 && sizeMb < (32 << 10));
  bs->Sb = 1;  // TODO
  bs->Nf = 2;
  bs->Sv = (sizeMb << 20) / bs->BytesPerSector;
  bs->Sf = bs->Sv / (bs->Sc << 7);  // TODO
  bs->RootCluster = DivRoundup(bs->Sb + bs->Nf * bs->Sf, bs->Sc);
  bs->FSInfo = 1;
  strcpy_s(bs->BS_FileSystemType, "FAT32");

  nRdetEntry = (bs->Sc * bs->BytesPerSector) >> 2;
  rdet.reset(new DirEntry *[nRdetEntry]);
  nFatEntry = (bs->Sf * bs->BytesPerSector) >> 2;
  afat.reset(new FatEntry[nFatEntry]);

  file.open(imgPath, std::ios::in | std::ios::out | std::ios::binary);
  file.seekp((sizeMb << 20) - 1);
  file.write("", 1);
  file.seekp(0);
  bs->Write(file);
}

void FAT32::RootWrite(std::filesystem::path const &srcPath) {
  size_t rdet_pos = bs->RootCluster * bs->Sc * bs->BytesPerSector;
  size_t rdet_nEntry = (bs->Sc * bs->BytesPerSector) >> 2;
  int nSrcClus = std::filesystem::file_size(srcPath) / bs->Sc;

  int nSubEntry = (srcPath.filename().u8string().length() + 12) / 13;
}

void FAT32::ReadRDET() {
  size_t rdet_pos = bs->RootCluster * bs->Sc * bs->BytesPerSector;
  DirEntry *rdet = new DirEntry[nRdetEntry];  // TODO maybe class var
  // Doc bang
  file.open() file.seekg(rdet_pos);
  for (int i = 0, nBlank = 0; i < nRdetEntry; ++i) {
    rdet[i].Read(inFile);
  }
}

void FAT32::ReadFAT() { size_t fat_pos = bs->Sb * bs->BytesPerSector; }
