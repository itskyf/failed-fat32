#include "FAT32.h"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <utility>

#include "utils.h"

int DivRoundup(int size, int n) {
  // Overflow-resistant
  int res = size / n;
  if (res * n != size) ++res;
  return res;
}

uint16_t CalClusterSize(size_t sizeMb) {
  using std::make_pair;
  // Pair mb - byte
  static const std::pair<int, uint16_t> VolClus[7] = {
      make_pair(33, 0),
      make_pair(65, 512),
      make_pair(129, 1 << 10),
      make_pair(257, 2 << 10),
      make_pair(8 << 10, 4 << 10),
      make_pair(16 << 10, 6 << 10),
      make_pair(32 << 10, 16 << 10),
  };
  for (int i = 0; i < 6; ++i) {
    if (sizeMb < VolClus[i].first) return VolClus[i].second;
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
  file.write(reinterpret_cast<char *>(&FSInfo), sizeof(FSInfo));
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
  memset(bs->BS_Jump_Code, 0, sizeof(bs->BS_Jump_Code));
  memset(bs->BS_OEM_ID, 0, sizeof(bs->BS_OEM_ID));

  bs->BytesPerSector = uint16_t(512);
  bs->Sc = 8;  // TODO CalClusSize(sizeMb);
  assert(sizeMb > 33 && sizeMb < (32 << 10));
  bs->Sb = 1;  // TODO
  bs->Nf = 2;
  bs->Sv = (sizeMb << 20) / bs->BytesPerSector;
  bs->Sf = bs->Sv / bs->Sc * 4 / bs->BytesPerSector;  // TODO
  bs->RootCluster = DivRoundup(bs->Sb + bs->Nf * bs->Sf, bs->Sc);
  bs->FSInfo = 1;
  strcpy_s(bs->BS_FileSystemType, "FAT32");

  CalcMember();
  rdet.reset(new DirEntry *[nRdetEntry]);
  afat.reset(new FatEntry[nFatEntry]);

  file.seekp((sizeMb << 20) - 1);
  file.write("", 1);
  file.seekp(0);
  bs->Write(file);

  file.open(imgPath, std::ios::in | std::ios::out | std::ios::binary);
}

FAT32::FAT32(std::string const &imgPath)
    : file(imgPath, std::ios::in | std::ios::out | std::ios::binary),
      bs(new BootSector) {
  if (!file.is_open()) {
    std::cout << "Vol doesnt EXIST\n";
    return;
  }
  bs->Read(file);
  CalcMember();
  rdet.reset(new DirEntry *[nRdetEntry]);
  afat.reset(new FatEntry[nFatEntry]);
  ReadRDET();
  ReadFAT();
  file.close();
}

void FAT32::CalcMember() {
  clusByte = bs->Sc * bs->BytesPerSector;
  fatPos = bs->Sb * bs->BytesPerSector;
  rdetPos = bs->RootCluster * clusByte;
  nRdetEntry = clusByte >> 2;
  nFatEntry = (bs->Sf * bs->BytesPerSector) >> 2;
  std::cout << "Byte/sector: " << bs->BytesPerSector << std::endl;
  std::cout << "Num clus: " << bs->Sv / bs->Sc << std::endl;
  std::cout << "Sc: " << int(bs->Sc) << std::endl;
  std::cout << "Sb: " << bs->Sb << std::endl;
  std::cout << "Sf: " << bs->Sf << std::endl;
  std::cout << "Nf: " << int(bs->Nf) << std::endl;
  std::cout << "Sv: " << bs->Sv << std::endl;
  std::cout << "RDET clus: " << bs->RootCluster << std::endl;
  std::cout << "Cluster in byte: " << clusByte << std::endl;
  std::cout << "Fat pos: " << fatPos << std::endl;
  std::cout << "RDET pos: " << rdetPos << std::endl;
  std::cout << "RDET entry: " << nRdetEntry << std::endl;
  std::cout << "FAT entry: " << nFatEntry << std::endl;
}

size_t FAT32::CalClusterPos(uint32_t clus) {
  return rdetPos + nRdetEntry << 2 + (clus - 2) * clusByte;
}

void FAT32::RootWrite(std::filesystem::path const &srcPath) {
  namespace fs = std::filesystem;
  assert(fs::exists(srcPath));
  int nSrcClus = DivRoundup(fs::file_size(srcPath), clusByte);
  int mSubEntry =
      (srcPath.filename().u16string().length() + 12) / 13;  // TOFIND

  // Find RDET empty entry
  int startCopDirEntry = -1;
  for (int i = 0, nEmpty = 0; i < nRdetEntry; ++i) {
    if (rdet[i]->isEmpty())
      ++nEmpty;
    else
      nEmpty = 0;
    if (nEmpty > mSubEntry) {
      startCopDirEntry = i - mSubEntry;
      break;
    }
  }
  assert(startCopDirEntry >= 0);

  // Find FAT empty entry
  std::vector<FatEntry *> emptyFatArr(nSrcClus);
  std::vector<int> emptyFatIdx(nSrcClus);
  emptyFatArr.back() = nullptr;
  for (int i = 2, n = 0; i < nFatEntry; ++i) {
    if (afat[i].state == FatTable::empty) {
      emptyFatArr[n++] = &afat[i];
      emptyFatIdx[n] = i;
    }
  }
  assert(emptyFatArr.back());
  for (size_t i = 0; i < size_t(nSrcClus) - 1; ++i) {
    emptyFatArr[i]->state = emptyFatIdx[i + 1];
  }
  emptyFatArr.back()->state = FatTable::eof;

  // Buoc 9 + 11
  dynamic_cast<MainEntry *>(rdet[startCopDirEntry])
      ->Init(srcPath.stem().u8string(), srcPath.extension().u8string(),
             fs::is_directory(srcPath) ? EntryAttribute::dir
                                       : EntryAttribute::file);
  WriteRDET();
  WriteFAT();

  std::ifstream fi(srcPath.filename().u16string(), std::ios::binary);
  char *buf = new char[clusByte];
  int i = 0;
  while (!fi.eof()) {
    file.seekp(CalClusterPos(emptyFatArr[i++]->state));
    fi.read(buf, clusByte);
    file.write(buf, clusByte);
  }
  delete[] buf;
  fi.close();
}

void FAT32::ReadRDET() {
  file.seekg(rdetPos);
  for (int i = 0; i < nRdetEntry; ++i) {
    char buf[32];
    file.read(buf, 32);
    if (DirEntry::isMain(buf)) {
      rdet[i] = new MainEntry(buf);
    } else {
      rdet[i] = new SubEntry(buf);
    }
  }
}

void FAT32::ReadFAT() {
  file.seekg(fatPos);
  file.read(reinterpret_cast<char *>(afat.get()), size_t(nFatEntry) << 2);
  // TOFIND
}

void FAT32::WriteRDET() {
  file.seekp(rdetPos);
  for (int i = 0; i < nRdetEntry; ++i) rdet[i]->Write(file);
}

void FAT32::WriteFAT() {
  file.seekp(fatPos);
  file.write(reinterpret_cast<char *>(afat.get()), size_t(nFatEntry) << 2);
}

std::vector<MainEntry *> FAT32::ReadRoot() {
  std::vector<MainEntry *> root;
  for (int i = 0; i < nRdetEntry; ++i) {
    if (rdet[i]->entryType == EntryType::main && !rdet[i]->isEmpty()) {
      MainEntry *mainEntryPtr = dynamic_cast<MainEntry *>(rdet[i]);
      root.push_back(mainEntryPtr);
    }
  }
  return root;
}
void FAT32::printRoot(std::vector<MainEntry *> root) {
  std::cout << "Flie name   "
            << "size (byte)" << std::endl;
  for (int i = 0; i < root.size(); i++) {
    std::cout << root[i]->filename << "\t" << root[i]->fileSize << std::endl;
  }
}

void FAT32::Password(std::string const &fileName) {}