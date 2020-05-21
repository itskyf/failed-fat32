#include "FAT32.h"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <utility>

#include "utils.h"

namespace fs = std::filesystem;

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

void BootSector::Write(std::ofstream &file) {
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

void BootSector::Read(std::ifstream &file) {
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
    : bs(new BootSector), rdet(nullptr), afat(nullptr), fName(imgPath) {
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

  std::ofstream fo(fName, std::ios::binary);
  fo.seekp((sizeMb << 20) - 1);
  fo.write("", 1);
  fo.seekp(0);
  bs->Write(fo);
  fo.close();
}

FAT32::FAT32(std::string const &imgPath) : fName(imgPath), bs(new BootSector) {
  std::ifstream fi(fName, std::ios::binary);
  if (!fi.is_open()) {
    std::cout << "File doesnt EXIST\n";
    throw;
  }
  bs->Read(fi);
  CalcMember();
  rdet.reset(new DirEntry *[nRdetEntry]);
  afat.reset(new FatEntry[nFatEntry]);
  ReadRDET(fi);
  ReadFAT(fi);
  fi.close();
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

std::wstring FAT32::GetLongName(int idx) {
  std::wstring tmp;
  while (rdet[--idx]->entryType == EntryType::sub) {
    SubEntry *sub = dynamic_cast<SubEntry *>(rdet[idx]);
    tmp += std::wstring(sub->name);
    if (sub->sequence_number_name & 1) break;
  }
  return tmp;
}

size_t FAT32::CalClusterPos(uint32_t clus) {
  return rdetPos + nRdetEntry << 2 + (clus - 2) * clusByte;
}

void FAT32::RootWrite(std::filesystem::path const &srcPath) {
  if (!fs::exists(srcPath)) {
    std::cout << "File not existed" << std::endl;
    return;
  }
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
      startCopDirEntry = i;
      break;
    }
  }
  assert(startCopDirEntry >= 0);

  // Find FAT empty entry
  std::vector<FatEntry *> emptyFatArr(nSrcClus);
  std::vector<int> emptyFatIdx(nSrcClus);
  emptyFatArr.back() = nullptr;
  for (int i = 2, n = 0; i < nFatEntry && n < nSrcClus; ++i) {
    if (afat[i].state == FatTable::fempty) {
      emptyFatArr[n] = &afat[i];
      emptyFatIdx[n++] = i;
    }
  }
  assert(emptyFatArr.back());
  for (size_t i = 0; i < size_t(nSrcClus) - 1; ++i) {
    emptyFatArr[i]->state = emptyFatIdx[i + 1];
  }
  emptyFatArr.back()->state = FatTable::eof;

  char shortName[11];
  ConvertShortName(srcPath, shortName);
  dynamic_cast<MainEntry *>(rdet[startCopDirEntry])
      ->Init(shortName,
             fs::is_directory(srcPath) ? EntryAttribute::dir
                                       : EntryAttribute::file,
             fs::file_size(srcPath));
  std::u16string longName = srcPath.filename().u16string();
  for (int i = 1; i < mSubEntry; ++i) {
    dynamic_cast<SubEntry *>(rdet[size_t(startCopDirEntry) - i])
        ->InitName(&longName[10 * (size_t(i) - 1)], i);
  }
  dynamic_cast<SubEntry *>(rdet[size_t(startCopDirEntry) - mSubEntry])
      ->InitName(&longName[10 * (size_t(mSubEntry) - 1)], mSubEntry,
                 longName.length() % 10);

  std::ofstream fo(fName, std::ios::binary | std::ios::out | std::ios::in);
  WriteRDET(fo);
  WriteFAT(fo);

  std::ifstream fi(srcPath.filename().u16string(), std::ios::binary);
  char *buf = new char[clusByte];
  int i = 0;
  while (!fi.eof()) {
    fi.read(buf, clusByte);
    fo.seekp(CalClusterPos(emptyFatArr[i++]->state));
    fo.write(buf, clusByte);
  }
  delete[] buf;
  fi.close();
  fo.close();
}

void FAT32::ReadRDET(std::ifstream &fi) {
  fi.seekg(rdetPos);
  for (int i = 0; i < nRdetEntry; ++i) {
    char buf[32];
    fi.read(buf, 32);
    if (DirEntry::isMain(buf)) {
      rdet[i] = new MainEntry(buf);
    } else {
      rdet[i] = new SubEntry(buf);
    }
  }
}

void FAT32::ReadFAT(std::ifstream &fi) {
  fi.seekg(fatPos);
  fi.read(reinterpret_cast<char *>(afat.get()), size_t(nFatEntry) << 2);
  // TOFIND
}

void FAT32::WriteRDET(std::ofstream &fo) {
  fo.seekp(rdetPos);
  for (int i = 0; i < nRdetEntry; ++i) {
    rdet[i]->Write(fo);
  }
}

void FAT32::WriteFAT(std::ofstream &fo) {
  fo.seekp(fatPos);
  fo.write(reinterpret_cast<char *>(afat.get()), size_t(nFatEntry) << 2);
}

std::vector<int> FAT32::ReadRoot() {
  std::vector<int> root;
  for (int i = 0; i < nRdetEntry; ++i) {
    if (rdet[i]->entryType == EntryType::main && !rdet[i]->isEmpty()) {
      MainEntry *mainEntryPtr = dynamic_cast<MainEntry *>(rdet[i]);
      root.push_back(i);
    }
  }
  return root;
}

void FAT32::printRoot(std::vector<int> mainIdx) {
  std::cout << "Flie name   "
            << "size (byte)" << std::endl;
  for (int i = 0; i < mainIdx.size(); i++) {
    std::wcout << GetLongName(mainIdx[i]) << "\t"
               << dynamic_cast<MainEntry *>(rdet[mainIdx[i]])->fileSize
               << std::endl;
  }
}

void FAT32::Password(std::string const &fileName) {}

MainEntry *FindFile(std::string const &fName) { return nullptr; }
