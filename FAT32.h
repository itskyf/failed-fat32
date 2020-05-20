#pragma once
#include <cstdint>
#include <fstream>
#include <memory>
#include <string>

#include "dir_entry.h"
#include "fat_entry.h"

// size 96
struct BootSector {
  uint8_t BS_Jump_Code[3];    // 3
  char BS_OEM_ID[8];          // 8
  uint16_t BytesPerSector;    // 2
  uint8_t Sc;                 // 1 sector/cluster
  uint16_t Sb;                // 2 sector thuoc Bootsector
  uint8_t Nf;                 // 1
  uint16_t RootEntryCount;    // 2
  uint16_t TotalSectors16;    // 2
  uint8_t Media;              // 1
  uint16_t FATSize16;         // 2
  uint16_t SectorsPerTrack;   // 2
  uint16_t NumberOfHeads;     // 2
  uint32_t HiddenSectors;     // 4
  uint32_t Sv;                // 4 size of volume
  uint32_t Sf;                // 4
  uint16_t ExtFlags;          // 2
  uint16_t FSVersion;         // 2
  uint32_t RootCluster;       // 4
  uint16_t FSInfo;            // 2
  uint16_t BkBootSec;         // 2
  uint8_t Reserved[12];       // 12
  uint8_t BS_DriveNumber;     // 1
  uint8_t BS_Reserved1;       // 1
  uint8_t BS_BootSig;         // 1
  uint32_t BS_VolumeID;       // 4
  char BS_VolumeLabel[11];    // 11
  char BS_FileSystemType[8];  // 8
  void Write(std::fstream &file);
  void Read(std::fstream &file);
};

class FAT32 {
 public:
  FAT32(size_t sizeMb, std::string const &imgPath);
  // FAT32(std::string const &imgPath) {}  // TODO
  virtual ~FAT32() = default;

 private:
  void RootWrite(std::filesystem::path const &srcPath);
  void ReadRDET();
  void ReadFAT();

  std::fstream file;
  std::unique_ptr<BootSector> bs;
  std::unique_ptr<DirEntry *> rdet;
  std::unique_ptr<FatEntry> afat;

  int nRdetEntry;
  int nFatEntry;
};