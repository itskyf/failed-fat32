#include "dir_entry.h"

bool DirEntry::isMain(char buf[32]) {
  return buf[0] != 0xE5 && buf[0xB] != 0x0F;
}

void MainEntry::Init(std::string const &shortName, std::string const &ext,
                     EntryAttribute _att) {
  memcpy(filename, shortName.c_str(), sizeof(filename));
  memcpy(extension, ext.c_str(), sizeof(extension));
  attribute = _att;
}

void MainEntry::Read(char buf[32]) {
  memcpy(filename, buf, 8);
  memcpy(extension, &buf[8], sizeof(extension));
  memcpy(&attribute, &buf[11], sizeof(attribute));
  memcpy(useless, &buf[12], sizeof(useless));
  memcpy(&modifiedTime, &buf[22], sizeof(modifiedTime));
  memcpy(&modifiedDate, &buf[24], sizeof(modifiedDate));
  memcpy(&firstCluster, &buf[26], sizeof(firstCluster));
  memcpy(&fileSize, &buf[28], sizeof(fileSize));
}

void MainEntry::Write(std::fstream &fo) {
  fo.write(reinterpret_cast<char *>(filename), sizeof(filename));
  fo.write(reinterpret_cast<char *>(extension), sizeof(extension));
  fo.write(reinterpret_cast<char *>(attribute), sizeof(attribute));
  fo.write(reinterpret_cast<char *>(useless), sizeof(useless));
  fo.write(reinterpret_cast<char *>(modifiedTime), sizeof(modifiedTime));
  fo.write(reinterpret_cast<char *>(modifiedDate), sizeof(modifiedDate));
  fo.write(reinterpret_cast<char *>(firstCluster), sizeof(firstCluster));
  fo.write(reinterpret_cast<char *>(fileSize), sizeof(fileSize));
}

bool MainEntry::isEmpty() { return !(*filename && *filename != 0xE5); }

void SubEntry::Read(char buf[32]) {
  memcpy(&sequence_number_name, buf, sizeof(sequence_number_name));
  memcpy(&sequence_number_pass, &buf[1], sizeof(sequence_number_pass));
  memcpy(&pass, &buf[2], sizeof(pass));
  memcpy(name, &buf[12], sizeof(name));
}

void SubEntry::Write(std::fstream &fo) {
  fo.write(reinterpret_cast<char *>(sequence_number_name),
           sizeof(sequence_number_name));
  fo.write(reinterpret_cast<char *>(sequence_number_pass),
           sizeof(sequence_number_pass));
  fo.write(reinterpret_cast<char *>(name), sizeof(name));
  fo.write(reinterpret_cast<char *>(pass), sizeof(pass));
}

bool SubEntry::isEmpty() {
  return sequence_number_name && sequence_number_name != 0xE5;
}
