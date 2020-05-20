#include "dir_entry.h"

void MainEntry::Read(std::ifstream &fi) {
  fi.read(reinterpret_cast<char *>(filename), sizeof(filename));
  fi.read(reinterpret_cast<char *>(extension), sizeof(extension));
  fi.read(reinterpret_cast<char *>(attributes), sizeof(attributes));
  fi.read(reinterpret_cast<char *>(useless), sizeof(useless));
  fi.read(reinterpret_cast<char *>(modifiedTime), sizeof(modifiedTime));
  fi.read(reinterpret_cast<char *>(modifiedDate), sizeof(modifiedDate));
  fi.read(reinterpret_cast<char *>(firstCluster), sizeof(firstCluster));
  fi.read(reinterpret_cast<char *>(fileSize), sizeof(fileSize));
}

void MainEntry::Write(std::ofstream &fo) {
  fo.write(reinterpret_cast<char *>(filename), sizeof(filename));
  fo.write(reinterpret_cast<char *>(extension), sizeof(extension));
  fo.write(reinterpret_cast<char *>(attributes), sizeof(attributes));
  fo.write(reinterpret_cast<char *>(useless), sizeof(useless));
  fo.write(reinterpret_cast<char *>(modifiedTime), sizeof(modifiedTime));
  fo.write(reinterpret_cast<char *>(modifiedDate), sizeof(modifiedDate));
  fo.write(reinterpret_cast<char *>(firstCluster), sizeof(firstCluster));
  fo.write(reinterpret_cast<char *>(fileSize), sizeof(fileSize));
}

void SubEntry::Read(std::ifstream &fi) {
  fi.read(reinterpret_cast<char *>(sequence_number_name),
          sizeof(sequence_number_name));
  fi.read(reinterpret_cast<char *>(sequence_number_pass),
          sizeof(sequence_number_pass));
  fi.read(reinterpret_cast<char *>(signature), sizeof(signature));
  fi.read(reinterpret_cast<char *>(name), sizeof(name));
  fi.read(reinterpret_cast<char *>(pass), sizeof(pass));
}

void SubEntry::Write(std::ofstream &fo) {
  fo.write(reinterpret_cast<char *>(sequence_number_name),
           sizeof(sequence_number_name));
  fo.write(reinterpret_cast<char *>(sequence_number_pass),
           sizeof(sequence_number_pass));
  fo.write(reinterpret_cast<char *>(name), sizeof(name));
  fo.write(reinterpret_cast<char *>(pass), sizeof(pass));
}
