#pragma once
#include <cstdint>
#include <fstream>

class DirEntry {
 public:
  virtual void Read(std::fstream &file) = 0;
  virtual void Write(std::fstream &file) = 0;
  enum EntryType { main = 0, sub } entryType;
};

class MainEntry : public DirEntry {
 public:
  void Read(std::ifstream &fi);
  void Write(std::ofstream &fo);

  uint8_t filename[8];    // 8
  uint8_t extension[3];   // 3
  uint8_t attributes;     // 1
  char useless[10];       // 10
  uint16_t modifiedTime;  // 2
  uint16_t modifiedDate;  // 2
  uint16_t firstCluster;  // 2
  uint32_t fileSize;      // 4
};

class SubEntry : public DirEntry {
 public:
  void Read(std::ifstream &fi);
  void Write(std::ofstream &fo);

  uint8_t sequence_number_name;  // 6 bit luu so thu tu, 2 bit ket thuc
  uint8_t sequence_number_pass;
  uint8_t signature = 0xF;  // TODO
  uint8_t name[15];
  uint8_t pass[14];
};
